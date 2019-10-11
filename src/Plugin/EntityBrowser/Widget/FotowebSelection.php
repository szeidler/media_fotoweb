<?php

namespace Drupal\media_fotoweb\Plugin\EntityBrowser\Widget;

use Drupal\Component\Render\FormattableMarkup;
use Drupal\Core\Config\ConfigFactoryInterface;
use Drupal\Core\Entity\EntityFieldManagerInterface;
use Drupal\Core\Entity\EntityTypeManagerInterface;
use Drupal\Core\Form\FormStateInterface;
use Drupal\Core\Logger\LoggerChannelFactoryInterface;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;
use Drupal\Core\Session\AccountInterface;
use Drupal\Core\Utility\Token;
use Drupal\entity_browser\WidgetBase;
use Drupal\entity_browser\WidgetValidationManager;
use Drupal\media_fotoweb\FotowebLoginManagerInterface;
use Symfony\Component\DependencyInjection\ContainerInterface;
use Symfony\Component\EventDispatcher\EventDispatcherInterface;

/**
 * Embeds the Fotoweb Selection Widget into Entity Browser.
 *
 * @EntityBrowserWidget(
 *   id = "fotoweb_selection",
 *   label = @Translation("Fotoweb"),
 *   description = @Translation("Fotoweb asset browser"),
 *   auto_select = FALSE
 * )
 */
class FotowebSelection extends WidgetBase implements ContainerFactoryPluginInterface {

  /**
   * Config factory service.
   *
   * @var \Drupal\Core\Config\ConfigFactoryInterface
   */
  protected $configFactory;

  /**
   * The entity field manager.
   *
   * @var \Drupal\Core\Entity\EntityFieldManagerInterface
   */
  protected $entityFieldManager;

  /**
   * The current user.
   *
   * @var \Drupal\Core\Session\AccountInterface
   */
  protected $currentUser;

  /**
   * The token service.
   *
   * @var \Drupal\Core\Utility\Token
   */
  protected $token;

  /**
   * The Fotoweb login manager.
   *
   * @var \Drupal\media_fotoweb\FotowebLoginManagerInterface
   */
  protected $loginManager;

  /**
   * The logger factory.
   *
   * @var \Drupal\Core\Logger\LoggerChannelFactoryInterface
   */
  protected $logger;

  /**
   * Constructs a new class instance.
   *
   * @param array $configuration
   *   A configuration array containing information about the plugin instance.
   * @param string $plugin_id
   *   The plugin_id for the plugin instance.
   * @param mixed $plugin_definition
   *   The plugin implementation definition.
   * @param \Symfony\Component\EventDispatcher\EventDispatcherInterface $event_dispatcher
   *   Event dispatcher service.
   * @param \Drupal\Core\Entity\EntityTypeManagerInterface $entity_type_manager
   *   Entity type manager service.
   * @param \Drupal\entity_browser\WidgetValidationManager $validation_manager
   *   The Widget Validation Manager service.
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   Config factory service.
   * @param \Drupal\Core\Entity\EntityFieldManagerInterface $entityFieldManager
   *   The entity field manager.
   * @param \Drupal\Core\Session\AccountInterface $current_user
   *   The current user.
   * @param \Drupal\Core\Utility\Token $token
   *   The token service.
   * @param \Drupal\media_fotoweb\FotowebLoginManagerInterface $login_manager
   *   The Fotoweb login manager.
   * @param \Drupal\Core\Logger\LoggerChannelFactoryInterface $logger_channel_factory
   *   Logger channel factory.
   */
  public function __construct(array $configuration, $plugin_id, $plugin_definition, EventDispatcherInterface $event_dispatcher, EntityTypeManagerInterface $entity_type_manager, WidgetValidationManager $validation_manager, ConfigFactoryInterface $config_factory, EntityFieldManagerInterface $entityFieldManager, AccountInterface $current_user, Token $token, FotowebLoginManagerInterface $login_manager, LoggerChannelFactoryInterface $logger_channel_factory) {
    parent::__construct($configuration, $plugin_id, $plugin_definition, $event_dispatcher, $entity_type_manager, $validation_manager);
    $this->configFactory = $config_factory;
    $this->entityFieldManager = $entityFieldManager;
    $this->currentUser = $current_user;
    $this->token = $token;
    $this->loginManager = $login_manager;
    $this->logger = $logger_channel_factory;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container, array $configuration, $plugin_id, $plugin_definition) {
    return new static(
      $configuration,
      $plugin_id,
      $plugin_definition,
      $container->get('event_dispatcher'),
      $container->get('entity_type.manager'),
      $container->get('plugin.manager.entity_browser.widget_validation'),
      $container->get('config.factory'),
      $container->get('entity_field.manager'),
      $container->get('current_user'),
      $container->get('token'),
      $container->get('media_fotoweb.login_manager'),
      $container->get('logger.factory')
    );
  }

  /**
   * {@inheritdoc}
   */
  public function getForm(array &$original_form, FormStateInterface $form_state, array $additional_widget_parameters) {
    $form = parent::getForm($original_form, $form_state, $additional_widget_parameters);

    $config = $this->configFactory->get('media_fotoweb.settings');

    $form['selection_widget'] = [
      '#type' => 'markup',
      '#markup' => $this->buildEmbeddedSelectionWidget(),
    ];
    $form['fotoweb_selected'] = [
      '#type' => 'hidden',
    ];

    $form['selection_widget']['#attached']['library'] = ['media_fotoweb/selection_widget'];
    $form['selection_widget']['#attached']['drupalSettings']['media_fotoweb']['host'] = $config->get('server');

    // Visually hide submit button. The asset selection will happen on click.
    $form['actions']['submit']['#attributes']['class'][] = 'visually-hidden';

    return $form;
  }

  /**
   * {@inheritdoc}
   */
  public function submit(array &$element, array &$form, FormStateInterface $form_state) {
    $entities = $this->prepareEntities($form, $form_state);
    $this->selectEntities($entities, $form_state);

    return FALSE;
  }

  /**
   * Builds the Fotoweb Embedded Selection Widget as an <iframe>.
   *
   * @return string
   *   Returns the embedded widget markup.
   */
  protected function buildEmbeddedSelectionWidget() {
    $config = $this->configFactory->get('media_fotoweb.settings');
    $fotoweb_host = $config->get('server');
    $widget_url = $fotoweb_host . '/fotoweb/widgets/selection';
    $widget_height = $config->get('selection_widget_height');

    // Generate user login token and append it to widget url, when using SSO.
    if ($config->get('selection_widget_use_sso')) {
      if ($user_login_token = $this->loginManager->getLoginTokenFromAccount($this->currentUser)) {
        $widget_url .= '?lt=' . $user_login_token;
      }
    }

    // Create the widget using an <iframe>.
    $build = new FormattableMarkup('<iframe src=":widget_url" width="100%" height=":widget_height"></iframe>', [
      ':widget_url' => $widget_url,
      ':widget_height' => $widget_height,
    ]);

    return $build;
  }

  /**
   * {@inheritdoc}
   */
  protected function prepareEntities(array $form, FormStateInterface $form_state) {
    $entity_type_id = 'fotoweb';
    $entities = [];
    $selected = json_decode($form_state->getValue('fotoweb_selected', []));

    /** @var \Drupal\media\MediaTypeInterface $media_type */
    $media_type = $this->entityTypeManager->getStorage('media_type')
      ->load($entity_type_id);
    /** @var \Drupal\media_fotoweb\Plugin\media\Source\Fotoweb $plugin */
    $plugin = $media_type->getSource();
    $source_field = $plugin->getConfiguration()['source_field'];

    foreach ($selected as $asset) {
      $mid = $this->entityTypeManager->getStorage('media')->getQuery()
        ->condition($source_field, $asset->href)
        ->range(0, 1)
        ->execute();
      if ($mid) {
        $media = $this->loadAndSyncMedia($mid, $asset);
        $entities[] = $media;
      }
      else {
        /** @var \Drupal\media\MediaInterface $media */
        $media = $this->entityTypeManager->getStorage('media')->create([
          'bundle' => $media_type->id(),
          $source_field => $asset->href,
          'uid' => $this->currentUser->id(),
          'status' => TRUE,
          'original_data' => $asset,
        ]);

        $media->save();
        $entities[] = $media;
      }
    }

    return $entities;
  }

  /**
   * Load and sync the existing media entity.
   *
   * @param int $mid
   *   The media id.
   * @param object $asset
   *   The Fotoweb asset.
   *
   * @return \Drupal\media\MediaInterface
   *   The selected media entity.
   */
  protected function loadAndSyncMedia($mid, object $asset) {
    $config = $this->configFactory->get('media_fotoweb.settings');
    $asset_update_type = $config->get('asset_update_type');

    /** @var \Drupal\media\MediaInterface $media */
    $media = $this->entityTypeManager->getStorage('media')
      ->load(reset($mid));

    // Resync the asset data from Fotoweb on selection, when "reuse" update
    // type was set.
    if ($asset_update_type === 'reused') {
      $media->original_data = $asset;
      $media->save();
    }

    return $media;
  }

}
