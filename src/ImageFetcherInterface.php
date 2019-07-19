<?php

namespace Drupal\media_fotoweb;

use Drupal\Component\Plugin\PluginInspectionInterface;

/**
 * Defines an interface for Fotoweb Image Fetcher Plugin plugins.
 */
interface ImageFetcherInterface extends PluginInspectionInterface {

  /**
   * Returns the scaled down rendition image for a given resource URL.
   *
   * @param string $resourceUrl
   *   The given resource URL.
   *
   * @return string|null
   *   The image file contents or NULL on failure.
   */
  public function getImageByResourceUrl($resourceUrl);

}
