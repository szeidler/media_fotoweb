<?php

namespace Drupal\media_fotoweb;

use Drupal\Component\Plugin\PluginBase;
use Drupal\Core\Plugin\ContainerFactoryPluginInterface;

/**
 * Base class for Fotoweb Image Fetcher plugins.
 */
abstract class ImageFetcherBase extends PluginBase implements ImageFetcherInterface, ContainerFactoryPluginInterface {

}
