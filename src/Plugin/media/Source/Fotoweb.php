<?php

namespace Drupal\media_fotoweb\Plugin\media\Source;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Field\FieldTypePluginManagerInterface;
use Drupal\Core\File\FileSystemInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Utility\Token;
use Drupal\file\FileInterface;
use Drupal\media\MediaSourceBase;
use Drupal\media\MediaTypeInterface;
use Drupal\media\MediaInterface;
use Drupal\media_fotoweb\Annotation\FotowebImageFetcher;
use Drupal\media_fotoweb\ImageFetcherManager;
use Drupal\media_fotoweb\OriginalImageFetcher;
use Drupal\media_fotoweb\RenditionImageFetcher;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Provides media type plugin for Fotoweb.
 *
 * @MediaSource(
 *   id = "fotoweb",
 *   label = @Translation("Fotoweb"),
 *   description = @Translation("Provides business logic and metadata for Fotoweb."),
 *   allowed_field_types = {
 *     "string",
 *     "string_long",
 *     "link"
 *   },
 *   default_thumbnail_filename = "image.png",
 *   default_name_metadata_attribute = "filename",
 * )
 */
class Fotoweb extends MediaSourceBase {

  /**
   * Config factory service.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * File system service.
   *
   * @var \Drupal\Core\File\FileSystemInterface
   */
  protected $fileSystem;

  /**
   * The token service.
   *
   * @var \Drupal\Core\Utility\Token
   */
  protected $token;

  /**
   * The Image Fetcher Plugin Manager.
   *
   * @var \Drupal\media_fotoweb\ImageFetcherManager
   */
  protected $imageFetcherManager;

  /**
   * Constructs a new class instance.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   Entity type manager service.
   * @param \Drupal\Core\Entity\EntityFieldManagerInterface $entity_field_manager
   *   The entity field manager.
   * @param \Drupal\Core\Field\FieldTypePluginManagerInterface $field_type_manager
   *   The field type plugin manager service.
   * @param \Drupal\Core\Config\ConfigFactoryInterface $configFactory
   *   The config factory service.
   * @param \Drupal\Core\File\FileSystemInterface $fileSystem
   *   The file system service.
   * @param \Drupal\Core\Utility\Token $token
   *   The token service.
   * @param \Drupal\media_fotoweb\ImageFetcherManager $image_fetcher_manager
   *   The Image Fetcher Plugin Manager
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EntityTypeManagerInterface $entity_type_manager, EntityFieldManagerInterface $entity_field_manager, FieldTypePluginManagerInterface $field_type_manager, ConfigFactoryInterface $configFactory, FileSystemInterface $fileSystem, Token $token, ImageFetcherManager $image_fetcher_manager) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $entity_type_manager, $entity_field_manager, $field_type_manager, $configFactory);
    $this->configFactory = $configFactory;
    $this->fileSystem = $fileSystem;
    $this->token = $token;
    $this->imageFetcherManager = $image_fetcher_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('entity_type.manager'),
      $container->get('entity_field.manager'),
      $container->get('plugin.manager.field.field_type'),
      $container->get('config.factory'),
      $container->get('file_system'),
      $container->get('token'),
      $container->get('plugin.manager.media_fotoweb.image_fetcher')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function defaultConfiguration() {
    return [
      'source_field' => '',
      'local_image' => '',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function getMetadataAttributes() {
    $fields = [
      'filename' => $this->t('Filename'),
      'file' => $this->t('File'),
      'href' => $this->t('Href'),
      'height' => $this->t('Height'),
      'created' => $this->t('Created'),
      'changed' => $this->t('Changed'),
    ];

    return $fields;

  }

  /**
   * {@inheritdoc}
   */
  public function getMetadata(MediaInterface $media, $name) {
    $value = NULL;
    $field_map = $media->bundle->entity->getFieldMap();

    switch ($name) {
      case 'default_name':
        return parent::getMetadata($media, 'default_name');

      case 'thumbnail_uri':
        return $this->getThumbnail($media);
    }

    if (isset($media->original_data) && $original_data = $media->original_data) {
      switch ($name) {
        case 'file':
          $file = $this->createOrGetFile($media);
          if (!empty($file) && $file instanceof FileInterface) {
            $value = $file->id();
          }
          break;

        default:
          $value = isset($original_data->{$name}) ? $original_data->{$name} : NULL;
      }
    }
    else {
      $media_values = $media->toArray();

      if (isset($media_values[$field_map[$name]])) {
        $value = $media_values[$field_map[$name]];
      }

    }

    return $value;
  }

  /**
   * {@inheritdoc}
   */
  public function buildConfigurationForm(array $form, FormStateInterface $form_state) {
    /** @var \Drupal\media\MediaTypeInterface $bundle */
    $bundle = $form_state->getFormObject()->getEntity();

    $form = parent::buildConfigurationForm($form, $form_state);
    $form['local_image'] = array(
      '#type' => 'select',
      '#title' => $this->t('Local image'),
      '#description' => $this->t('Field on media entity that stores Image file. You can create a bundle without selecting a value for this dropdown initially. This dropdown can be populated after adding fields to the bundle.'),
      '#default_value' => empty($this->configuration['local_image']) ? NULL : $this->configuration['local_image'],
      '#options' => $this->getLocalImageFieldOptions($bundle),
    );

    return $form;
  }

  /**
   * Build the local image field options.
   *
   * @param \Drupal\media\MediaTypeInterface $bundle
   *   The bundle of the configuration form.
   *
   * @return array
   *   Possible local image fields as a key/value pair.
   */
  protected function getLocalImageFieldOptions(MediaTypeInterface $bundle) {
    $options = [];
    $allowed_field_types = ['image'];

    foreach ($this->entityFieldManager->getFieldDefinitions('media', $bundle->id()) as $field_name => $field) {
      if (in_array($field->getType(), $allowed_field_types) && !$field->getFieldStorageDefinition()
          ->isBaseField()) {
        $options[$field_name] = $field->getLabel();
      }
    }

    return $options;
  }

  /**
   * Returns the Thumbnail for a given media.
   *
   * @param \Drupal\media\MediaInterface $media
   *   The media entity to get a file for.
   *
   * @return string
   *   The file URI.
   */
  public function getThumbnail(MediaInterface $media) {
    $file = $this->createOrGetFile($media);
    if (!empty($file) && $file instanceof FileInterface) {
      // If the file is new, set it directly to the local image field, because
      // the thumbnail function is processed earlier.
      $file_field = $this->getLocalFileField($media);
      if ($media->hasField($file_field) && $media->{$file_field}->isEmpty()) {
        $media->set($file_field, $file->id());
      }
      return $file->getFileUri();
    }

    return parent::getMetadata($media, 'thumbnail_uri');

  }

  /**
   * Returns an associated file or creates a new one.
   *
   * @param \Drupal\media\MediaInterface $media
   *   The media entity to get a file for.
   *
   * @return bool|\Drupal\file\FileInterface
   *   A file entity or FALSE on failure.
   */
  protected function createOrGetFile(MediaInterface $media) {
    // If there is already a file on the media entity then we should use that.
    $file = $this->getExistingFile($media);

    // TODO: Find out if it is an updated version -> comparing the modified timestamp.
    $is_updated_version = FALSE;

    if (empty($file)) {
      $replace = $is_updated_version ?
        FileSystemInterface::EXISTS_REPLACE :
        FileSystemInterface::EXISTS_RENAME;
      $file = $this->createNewFile($media, $replace);
    }

    return $file;
  }

  /**
   * @param \Drupal\media\MediaInterface $media
   *   The media entity to get the existing file ID from.
   *
   * @return bool|\Drupal\file\FileInterface
   *   The existing file or FALSE if one was not found.
   */
  protected function getExistingFile(MediaInterface $media) {
    $file_field = $this->getLocalFileField($media);
    if ($media->hasField($file_field)) {
      /** @var \Drupal\file\Plugin\Field\FieldType\FileItem $file */
      $file_item = $media->get($file_field)->first();
      if (!empty($file_item->target_id)) {
        return $this->entityTypeManager->getStorage('file')
          ->load($file_item->target_id);
      }
    }

    return FALSE;
  }

  /**
   * Creates a new file for a fotoweb asset.
   *
   * @param \Drupal\media\MediaInterface $media
   *   The media entity.
   * @param int $replace
   *   Flag for dealing with existing files.
   *
   * @return bool|\Drupal\file\FileInterface
   *   The created file or FALSE on failure.
   */
  protected function createNewFile(MediaInterface $media, $replace = FileSystemInterface::EXISTS_RENAME) {
    // Ensure we can write to our destination directory.
    $destination_folder = $this->getLocalFileDirectory($media);
    $destination_name = $this->getMetadata($media, 'filename');
    $destination_path = sprintf('%s/%s', $destination_folder, $destination_name);
    if (!$this->fileSystem->prepareDirectory($destination_folder, FileSystemInterface::CREATE_DIRECTORY | FileSystemInterface::MODIFY_PERMISSIONS)) {
      return FALSE;
    }

    // Get the file from Fotoweb.
    $config = $this->configFactory->get('media_fotoweb.settings');
    $storageType = $config->get('file_storage_type');
    $imageFetcher = $this->imageFetcherManager->createInstance($storageType);
    $resourceUrl = $this->getMetadata($media, 'href');
    $data = $imageFetcher->getImageByResourceUrl($resourceUrl);
    $file = file_save_data($data, $destination_path, $replace);

    return $file;
  }

  /**
   * Gets the destination path for Acquia DAM assets.
   *
   * @param \Drupal\Media\MediaInterface $media
   *   The media entity to get file field information from.
   *
   * @return string
   *   The final folder to store the fotoweb asset locally.
   */
  protected function getLocalFileDirectory(MediaInterface $media) {
    $scheme = \file_default_scheme();
    $file_directory = 'fotoweb';

    // Get the file field settings and use its directory storage information.
    $file_field = $this->getLocalFileField($media);
    if (!empty($file_field)) {
      // Load the field definitions for this bundle.
      $field_definitions = $this->entityFieldManager->getFieldDefinitions($media->getEntityTypeId(), $media->bundle());
      // Get the storage scheme for the file field.
      $scheme = $field_definitions[$file_field]->getItemDefinition()
        ->getSetting('uri_scheme');
      // Get the file directory for the file field.
      $file_directory = $field_definitions[$file_field]->getItemDefinition()
        ->getSetting('file_directory');
      // Replace the token for file directory.
      if (!empty($file_directory)) {
        $file_directory = $this->token->replace($file_directory);
      }
    }

    return sprintf('%s://%s', $scheme, $file_directory);
  }

  /**
   * Gets the file field being used to store the fotoweb asset.
   *
   * @param \Drupal\media\MediaInterface $media
   *   The media entity to get the mapped file field.
   *
   * @return bool|string
   *   The name of the file field on the media bundle or FALSE on failure.
   */
  protected function getLocalFileField(MediaInterface $media) {
    /** @var \Drupal\media\MediaTypeInterface $media_type */
    $media_type = $this->entityTypeManager->getStorage('media_type')->load($media->bundle());
    $source = $media_type->getSource();
    return $source->getConfiguration()['local_image'];
  }

}
