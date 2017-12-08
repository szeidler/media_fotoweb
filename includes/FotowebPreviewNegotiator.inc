<?php

use Fotoweb\Representation\Asset;

/**
 * FotowebPreviewNegotiator negotiates the best fitting image.
 */
class FotowebPreviewNegotiator {

  protected $asset;

  protected $localFileThreshold;

  /**
   * FotowebPreviewNegotiator constructor.
   *
   * @param Asset $asset
   *   Currently processed Fotoweb asset.
   * @param $localFileThreshold
   *   Minimal size for which the class should return the best fitting image.
   */
  public function __construct(Asset $asset, $localFileThreshold) {
    if (!is_numeric($localFileThreshold)) {
      throw new InvalidArgumentException();
    }
    $this->asset = $asset;
    $this->localFileThreshold = $localFileThreshold;
  }


  /**
   * Return the best fit asset image path from a asset resource.
   *
   * @return string|null
   *   Image path of the best fitting image.
   */
  public function getBestFitImagePath() {
    // Iterate over previews and use the first preview image that matches
    // the specified threshold.
    if ($previews = $this->asset->offsetGet('previews')) {
      $originalImageWidth = $this->asset->offsetGet('attributes')['imageattributes']['pixelwidth'];

      $previews = $this->orderPreviewsDescendant($previews);

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
      return strcmp($b['width'], $a['width']);
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
    // the original image dimensions and bitter than the localFileThreshold.
    if ($this->localFileThreshold) {
      return ($currentImage['width'] > $this->localFileThreshold && $currentImage['width'] <= $originalImageWidth);
    }
    // When no threshold was specified, the largest preview, that is not
    // exceeding the original dimensions is the better fit.
    else {
      return ($currentImage['width'] > $bestFitImage['width'] && $currentImage['width'] <= $originalImageWidth);
    }
  }

}
