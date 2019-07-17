<?php

namespace Drupal\media_fotoweb\Annotation;

use Drupal\Component\Annotation\Plugin;

/**
 * Defines a Fotoweb Image Fetcher Plugin item annotation object.
 *
 * @see \Drupal\media_fotoweb\ImageFetcherManager
 * @see plugin_api
 *
 * @Annotation
 */
class FotowebImageFetcher extends Plugin {


  /**
   * The plugin ID.
   *
   * @var string
   */
  public $id;

  /**
   * The label of the plugin.
   *
   * @var \Drupal\Core\Annotation\Translation
   *
   * @ingroup plugin_translatable
   */
  public $label;

  /**
   * The weight of the plugin in its group.
   *
   * @var int
   */
  public $weight;

}
