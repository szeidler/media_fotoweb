<?php

/**
 * Class FotowebAsset
 *
 * @see https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/Asset_representation
 */
class FotowebAsset implements FotowebRepresentationInterface {

  protected $fotoweb;
  protected $data;
  protected $localFileThreshold;

  const headerAccept = 'application/vnd.fotoware.asset+json';

  public function __construct(FotowebBase $fotoweb, $localFileThreshold = NULL) {
    $this->fotoweb = $fotoweb;
    $this->localFileThreshold = $localFileThreshold;
  }


  /**
   * @inheritdoc
   */
  public function initiateRequest($resourceUrl) {
    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->initiateRequest($request);
  }

  /**
   * Returns the asset for a given Fotoweb ressource.
   *
   * @param $resourceUrl
   *   Url of the asset.
   *
   * @return mixed
   *   Array representation of the asset.
   */
  public function getAsset($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    return json_decode($response->getBody(TRUE), TRUE);
  }

  /**
   * Returns the preview image path for a given Fotoweb resource.
   *
   * @param $resourceUrl
   *   Url of the asset.
   *
   * @return mixed
   *   Image URL.
   */
  public function getAssetImagePathFromResource($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    $this->data = json_decode($response->getBody(TRUE), TRUE);
    $image = $this->getAssetPreviewImageFromAsset();
    return $image['href'];
  }

  /**
   * Returns the preview image for a given Fotoweb resource.
   *
   * @param $resourceUrl
   *   Url of the asset.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response consisting of the image.
   */
  public function getAssetImageFromResource($resourceUrl) {
    $imagePath = $this->getAssetImagePathFromResource($resourceUrl);
    return $this->getAssetImageFromUrl($imagePath);
  }

  /**
   * Requests the image from a given URL.
   *
   * @param $imagePath
   *   Given image url.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   *   HTTP response consisting of the image.
   */
  public function getAssetImageFromUrl($imagePath) {
    // Request the designated preview image.
    $request = new GuzzleHttp\Psr7\Request('GET', $imagePath);
    $response = $this->fotoweb->initiateRequest($request);
    return $response;
  }

  /**
   * Return the asset image from a asset resource.
   *
   * @return mixed|null|\Psr\Http\Message\ResponseInterface
   *   HTTP response for the best fitting preview image.
   */
  private function getAssetPreviewImageFromAsset() {
    // Iterate over previews and use the first preview image that matches the specified threshold.
    if (!empty($this->data['previews'])) {
      $previews = $this->data['previews'];
      $originalImageWidth = $this->data['attributes']['imageattributes']['pixelwidth'];

      $previews = $this->orderPreviewsDescendant($previews);

      // Set initial bestFitImage
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
   * @param $previews
   *   Array of image previews properties.
   *
   * @return mixed
   *   The descendant ordered image previews.
   */
  public function orderPreviewsDescendant($previews) {
    usort($previews, function ($a, $b) {
      return strcmp($b['width'], $a['width']);
    });
    return $previews;
  }

  /**
   * Checks, if the current iterated image, is a better fit, dependent on
   * maximum local file size.
   *
   * @param $currentImage
   *   Currently processed image.
   * @param $bestFitImage
   *   Current best fit image.
   * @param $originalImageWidth
   *   Width of the original image.
   *
   * @return bool
   *   True if the current image is a better fit.
   */
  public function isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth) {
    // Square previews cannot be a good fit, we need the original aspect ratio.
    if (isset($currentImage['square']) && $currentImage['square']) return FALSE;

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
