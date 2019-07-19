<?php

namespace Drupal\media_fotoweb\Form;

use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Form\ConfigFormBase;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Url;
use Drupal\media_fotoweb\ImageFetcherManager;
use GuzzleHttp\Exception\RequestException;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * Configure media_fotoweb settings for this site.
 */
class FotowebSettingsForm extends ConfigFormBase {

  /**
   * The entity field manager.
   *
   * @var \Drupal\Core\Entity\EntityFieldManagerInterface
   */
  protected $entityFieldManager;

  /**
   * The Image Fetcher Manager.
   *
   * @var \Drupal\media_fotoweb\ImageFetcherManager
   */
  protected $imageFetcherManager;

  /**
   * Constructs a FotowebSettingsForm object.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The factory for configuration objects.
   * @param \Drupal\Core\Entity\EntityFieldManagerInterface $entity_field_manager
   *   The entity field manager.
   * @param \Drupal\media_fotoweb\ImageFetcherManager $image_fetcher_manager
   *   The image fetcher manager.
   */
  public function __construct(ConfigFactoryInterface $config_factory, EntityFieldManagerInterface $entity_field_manager, ImageFetcherManager $image_fetcher_manager) {
    $this->setConfigFactory($config_factory);
    $this->entityFieldManager = $entity_field_manager;
    $this->imageFetcherManager = $image_fetcher_manager;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory'),
      $container->get('entity_field.manager'),
      $container->get('plugin.manager.media_fotoweb.image_fetcher')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getFormId() {
    return 'media_fotoweb_settings';
  }

  /**
   * {@inheritdoc}
   */
  protected function getEditableConfigNames() {
    return [
      'media_fotoweb.settings',
    ];
  }

  /**
   * {@inheritdoc}
   */
  public function buildForm(array $form, FormStateInterface $form_state) {
    $config = $this->config('media_fotoweb.settings');

    $form['server'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Fotoweb server'),
      '#description' => $this->t('Use the server address, including protocol, excluding relative paths (/fotoweb) and trailing slashes. Example: https://fotoweb.mydomain.no'),
      '#default_value' => $config->get('server'),
      '#required' => TRUE,
    ];

    $form['full_api_key'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Full API Key'),
      '#description' => $this->t('This module is using a Full Server-to-server API Authentication. See <a href="@documentation_url" target="_blank">the Fotoweb documentation</a> for more information.', ['@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/Setting_the_API_key_in_the_Operations_Center']),
      '#default_value' => $config->get('full_api_key'),
      '#required' => TRUE,
    ];

    $form['selection_widget_use_sso'] = [
      '#type' => 'checkbox',
      '#title' => $this->t('Use SSO for the asset selection widget?'),
      '#description' => $this->t('Enable this, when you want to use the <a href="@documentation_url" target="_blank">Single Sign-on</a> function to automatically authenticate your Drupal user to the Fotoweb widget. Users need to set their Fotoweb username in their Drupal user profile, to make the Single Sign-on work.', ['@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/Integrating_FotoWeb_with_third-party_systems/User_Authentication_for_Embeddable_Widgets#Single_Sign_On_(SSO)_for_Widgets']),
      '#default_value' => $config->get('selection_widget_use_sso'),
    ];

    $form['encryption_secret'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Encryption secret'),
      '#description' => $this->t('Single-Sign on requires an <a href="@documentation_url" target="_blank">encryption secret</a> for authenticating the users.', ['@documentation_url' => 'https://learn.fotoware.com/02_FotoWeb_8.0/05_Configuring_sites/Finding_and%2F%2For_changing_the_encryption_secret']),
      '#default_value' => $config->get('encryption_secret'),
      '#states' => [
        'visible' => [
          ':input[name="selection_widget_use_sso"]' => ['checked' => TRUE],
        ],
        'required' => [
          ':input[name="selection_widget_use_sso"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['sso_user_field'] = [
      '#type' => 'select',
      '#title' => $this->t('Fotoweb Username Field'),
      '#description' => $this->t('Select the field, that stores your users Fotoweb username (used in the Fotoweb system) for performing the Single Sign-on. If you find no appropriate field, create one on the <a href="@user_field_ui">user field configuration</a>.', ['@user_field_ui' => Url::fromRoute('entity.user.field_ui_fields')->toString()]),
      '#options' => $this->getUserSingleSignOnFieldsAsOptions(),
      '#default_value' => $config->get('sso_user_field'),
      '#states' => [
        'visible' => [
          ':input[name="selection_widget_use_sso"]' => ['checked' => TRUE],
        ],
      ],
    ];

    $form['selection_widget_height'] = [
      '#type' => 'number',
      '#title' => $this->t('Selection Widget Height'),
      '#description' => $this->t('Specify the height of the selection widget in pixels.'),
      '#default_value' => $config->get('selection_widget_height'),
    ];

    $form['file_storage_type'] = [
      '#type' => 'select',
      '#title' => $this->t('File storage type'),
      '#description' => $this->t('Original images from Fotoweb might be unnecessary big for your website usage. You can either store the original image or a smaller appropriate preview with your desired maximum width.'),
      '#options' => $this->imageFetcherManager->getImageFetcherOptionList(),
      '#default_value' => $config->get('file_storage_type'),
    ];

    $form['local_file_size_threshold'] = [
      '#type' => 'textfield',
      '#title' => $this->t('Local file size threshold'),
      '#description' => $this->t('Define the minimal size for your locally stored images. The module will import the appropriate preview size from Fotoweb using the first preview, that matches the minimum threshold. Beware: The original image (and so the maximum) might be smaller than the threshold.'),
      '#default_value' => $config->get('local_file_size_threshold'),
      '#states' => [
        'visible' => [
          ':input[name="file_storage_type"]' => ['value' => 'rendition_image'],
        ],
        'required' => [
          ':input[name="file_storage_type"]' => ['value' => 'rendition_image'],
        ],
      ],
    ];

    $form['asset_update_type'] = [
      '#type' => 'select',
      '#title' => $this->t('When should the asset be updated?'),
      '#description' => $this->t('Define, when the asset metadata should imported. <br><em>On new created assets:</em> Only when the asset was used the first time the metadata will be imported. <br><em>On every asset selection:</em> Whenever an user selects an asset the metadata will be updated. This option can have implications (changed metadata) on old articles, that are using the same asset.'),
      '#options' => [
        'new' => $this->t('On new created assets'),
        'reused' => $this->t('On every asset selection'),
      ],
      '#default_value' => $config->get('asset_update_type'),
    ];

    return parent::buildForm($form, $form_state);
  }

  /**
   * {@inheritdoc}
   */
  public function submitForm(array &$form, FormStateInterface $form_state) {
    $config = $this->config('media_fotoweb.settings');
    $config->set('server', $form_state->getValue('server'));
    $config->set('full_api_key', $form_state->getValue('full_api_key'));
    $config->set('selection_widget_use_sso', $form_state->getValue('selection_widget_use_sso'));
    $config->set('encryption_secret', $form_state->getValue('encryption_secret'));
    $config->set('sso_user_field', $form_state->getValue('sso_user_field'));
    $config->set('selection_widget_height', $form_state->getValue('selection_widget_height'));
    $config->set('file_storage_type', $form_state->getValue('file_storage_type'));
    $config->set('local_file_size_threshold', $form_state->getValue('local_file_size_threshold'));
    $config->set('asset_update_type', $form_state->getValue('asset_update_type'));

    /** @var \Drupal\media_fotoweb\FotowebClient $client */
    $client = \Drupal::service('media_fotoweb.client');
    $clientConfiguration = [
      'baseUrl' => $form_state->getValue('server'),
      'apiToken' => $form_state->getValue('full_api_key'),
      'client_config' => ['allow_redirects' => FALSE],
    ];
    $client->createClientFromConfiguration($clientConfiguration);
    try {
      if ($rendition_service = $client->fetchRenditionService()) {
        $config->set('rendition_service', $rendition_service);
      }
      else {
        $this->messenger()
          ->addWarning($this->t('No rendition service found. Therefore you cannot fetch original images. You might need to check your Fotoweb server configuration.'));
      }
    }
    catch (RequestException $e) {
      $errorMessage = $e->getMessage();
      $this->messenger()
        ->addError($this->t('There was an networking error: @error_message', ['@error_message' => $errorMessage]));
    }
    catch (\Exception $e) {
      $errorMessage = $e->getMessage();
      $this->messenger()
        ->addError($this->t('@error_message', ['@error_message' => $errorMessage]));
    }

    $config->save();

    parent::submitForm($form, $form_state);
  }

  /**
   * Returns the user SSO field option list.
   *
   * @return array
   *   Field option list as key/value pair.
   */
  protected function getUserSingleSignOnFieldsAsOptions() {
    // If there are existing fields to choose from, allow the user to reuse one.
    $options = [];
    foreach ($this->entityFieldManager->getFieldDefinitions('user', 'user') as $field_name => $field) {
      $allowed_type = in_array($field->getType(), $this->getAllowedSingleSignOnFieldTypes(), TRUE);
      // Provide only fields that are from an allowed field type, not a base
      // field or the name field.
      if ($allowed_type && (!$field->getFieldStorageDefinition()->isBaseField() || $field->getName() === 'name')) {
        $options[$field_name] = $field->getLabel();
      }
    }
    return $options;
  }

  /**
   * Returns the list of allowed field types for the SSO mapping.
   *
   * @return array
   *   A list of field types.
   */
  protected function getAllowedSingleSignOnFieldTypes() {
    return ['string'];
  }

}
