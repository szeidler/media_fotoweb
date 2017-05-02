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


  public function initiateRequest($resourceUrl) {
    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->initiateRequest($request);
  }

  public function getAsset($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    return json_decode($response->getBody(TRUE), TRUE);
  }

  public function getAssetImagePathFromResource($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    $this->data = json_decode($response->getBody(TRUE), TRUE);
    $image = $this->getAssetPreviewImageFromAsset();
    return $image['href'];
  }

  public function getAssetImageFromResource($resourceUrl) {
    $imagePath = $this->getAssetImagePathFromResource($resourceUrl);
    return $this->getAssetImageFromUrl($imagePath);
  }

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
   */
  private function getAssetPreviewImageFromAsset() {
    // Iterate over previews and use the first preview image that matches the specified threshold.
    if (!empty($this->data['previews'])) {
      $previews = $this->data['previews'];
      $originalImageWidth = $this->data['attributes']['imageattributes']['pixelwidth'];

      // Order the previews in a descendant size order.
      usort($previews, function ($a, $b) {
        return strcmp($b['width'], $a['width']);
      });

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
   * Checks, if the current iterated image, is a better fit, dependent on
   * maximum local file size.
   *
   * @param $currentImage
   * @param $bestFitImage
   * @param $originalImageWidth
   *
   * @return bool
   */
  public function isImageABetterFit($currentImage, $bestFitImage, $originalImageWidth) {
    // Square previews cannot be a good fit, we need the original aspect ratio.
    if ($currentImage['square']) return FALSE;

    // The current image is the bestFit, it is smaller or equal than
    // the original image dimensions and bitter than the localFileThreshold.
    if ($this->localFileThreshold) {
      return ($currentImage['size'] > $this->localFileThreshold && $currentImage['size'] <= $originalImageWidth);
    }
    // When no threshold was specified, the largest preview, that is not
    // exceeding the original dimensions is the better fit.
    else {
      return ($currentImage['size'] > $bestFitImage['size'] && $currentImage['size'] <= $originalImageWidth);
    }
  }

}


