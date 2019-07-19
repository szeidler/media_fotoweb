<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Config\ConfigFactoryInterface;
use Fotoweb\Representation\Asset;
use Symfony\Component\DependencyInjection\ContainerInterface;

/**
 * RenditionNegotiator negotiates the best fitting image.
 */
class RenditionNegotiator {

  /**
   * Defines the threshold in pixels which is used for finding the best fit.
   *
   * @var int
   */
  protected $localFileThreshold;

  /**
   * RenditionNegotiator constructor.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   */
  public function __construct(ConfigFactoryInterface $config_factory) {
    $this->localFileThreshold = $config_factory->get('media_fotoweb.settings')->get('local_file_size_threshold');
  }

  /**
   * @return int
   */
  public function getLocalFileThreshold() {
    return $this->localFileThreshold;
  }

  /**
   * @param int $localFileThreshold
   */
  public function setLocalFileThreshold($localFileThreshold) {
    $this->localFileThreshold = $localFileThreshold;
  }

  /**
   * {@inheritdoc}
   */
  public static function create(ContainerInterface $container) {
    return new static(
      $container->get('config.factory')
    );
  }

  /**
   * Return the best fit asset image path from a asset resource.
   *
   * @param \Fotoweb\Representation\Asset $asset
   *   The Fotoweb asset.
   *
   * @return string|null
   *   Image path of the best fitting image.
   */
  public function getBestFitImagePath(Asset $asset) {
    // Iterate over previews and use the first preview image that matches
    // the specified threshold.
    if ($previews = $asset->offsetGet('previews')) {
      $originalImageWidth = $asset->offsetGet('attributes')['imageattributes']['pixelwidth'];

      $previews = $this->orderPreviewsAscendant($previews);

      // Set initial bestFitImage.
      $bestFitImage = reset($previews);

      // Iterate over previews to find the bestFit.
      foreach ($previews as $currentPreviewImage) {
        if ($this->isImageABetterFit($currentPreviewImage, $bestFitImage, $originalImageWidth)) {
          $bestFitImage = $currentPreviewImage;
        }
      }
      return $bestFitImage;
    }

    return NULL;
  }

  /**
   * Orders the previews in a ascendant size order.
   *
   * @param array $previews
   *   Array of image previews properties.
   *
   * @return array
   *   The ascendant ordered image previews.
   */
  public function orderPreviewsAscendant(array $previews) {
    usort($previews, function ($a, $b) {
      return ($a['width'] > $b['width']);
    });
    return $previews;
  }

  /**
   * Orders the previews in a descendant size order.
   *
   * @param array $previews
   *   Array of image previews properties.
   *
   * @return array
   *   The descendant ordered image previews.
   */
  public function orderPreviewsDescendant(array $previews) {
    usort($previews, function ($a, $b) {
      return ($a['width'] < $b['width']);
    });
    return $previews;
  }

  /**
   * Checks, if the current iterated image, is a better fit.
   *
   * That is dependent on maximum local file size.
   *
   * @param array $currentImage
   *   Currently processed image.
   * @param array $bestFitImage
   *   Current best fit image.
   * @param int $originalImageWidth
   *   Width of the original image.
   *
   * @return bool
   *   True if the current image is a better fit.
   */
  public function isImageABetterFit(array $currentImage, array $bestFitImage, $originalImageWidth) {
    // Square previews cannot be a good fit, we need the original aspect ratio.
    if (isset($currentImage['square']) && $currentImage['square']) {
      return FALSE;
    }

    // The current image is the bestFit, it is smaller or equal than
    // the original image dimensions and better than the localFileThreshold.
    if ($this->localFileThreshold) {
      return ($bestFitImage['width'] < $this->localFileThreshold && $currentImage > $this->localFileThreshold && $currentImage['width'] <= $originalImageWidth);
    }
    // When no threshold was specified, the largest preview, that is not
    // exceeding the original dimensions is the better fit.
    else {
      return ($currentImage['width'] > $bestFitImage['width'] && $currentImage['width'] <= $originalImageWidth);
    }
  }

}
