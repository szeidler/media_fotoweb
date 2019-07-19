<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Plugin\DefaultPluginManager;
use Drupal\Core\Cache\CacheBackendInterface;
use Drupal\Core\Extension\ModuleHandlerInterface;

/**
 * Provides the Fotoweb Image Fetcher plugin manager.
 */
class ImageFetcherManager extends DefaultPluginManager {

  /**
   * Constructs a new ImageFetcherManager object.
   *
   * @param \Traversable $namespaces
   *   An object that implements \Traversable which contains the root paths
   *   keyed by the corresponding namespace to look for plugin implementations.
   * @param \Drupal\Core\Cache\CacheBackendInterface $cache_backend
   *   Cache backend instance to use.
   * @param \Drupal\Core\Extension\ModuleHandlerInterface $module_handler
   *   The module handler to invoke the alter hook with.
   */
  public function __construct(\Traversable $namespaces, CacheBackendInterface $cache_backend, ModuleHandlerInterface $module_handler) {
    parent::__construct('Plugin/ImageFetcher', $namespaces, $module_handler, 'Drupal\media_fotoweb\ImageFetcherInterface', 'Drupal\media_fotoweb\Annotation\FotowebImageFetcher');

    $this->alterInfo('media_fotoweb_image_fetcher_info');
    $this->setCacheBackend($cache_backend, 'media_fotoweb_image_fetcher_plugins');
  }

  /**
   * {@inheritdoc}
   */
  protected function findDefinitions() {
    $definitions = parent::findDefinitions();
    uasort($definitions, [
      'Drupal\Component\Utility\SortArray',
      'sortByWeightElement',
    ]);
    return $definitions;
  }

  /**
   * Get an options list for all available image fetchers.
   *
   * @return array
   *   An array of options keyed by plugin ID with label values.
   */
  public function getImageFetcherOptionList() {
    $options = [];
    foreach ($this->getDefinitions() as $definition) {
      $options[$definition['id']] = $definition['label'];
    }
    return $options;
  }

}
