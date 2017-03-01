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
    $this->fotoweb->ensureAuthentication();

    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->request($request);
  }

  public function getAssetImageFromResource($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    $this->data = drupal_json_decode($response->getBody(TRUE), TRUE);
    $response = $this->getAssetImageFromAsset();
    return $response;
  }

  private function getAssetImageFromAsset() {
    // Iterate over previews and use the largest preview image.
    // TODO see, for a way to retrieve the original image for those who need it.
    if (!empty($this->data['previews'])) {
      $bestFitImage = array('size' => 0);
      foreach ($this->data['previews'] as $currentPreviewImage) {
        if (!$currentPreviewImage['square'] && $this->isBestFitImage($currentPreviewImage, $bestFitImage)) {
          $bestFitImage = $currentPreviewImage;
        }
      }

      // Request the designated preview image.
      $request = new GuzzleHttp\Psr7\Request('GET', $bestFitImage['href']);
      $response = $this->fotoweb->request($request);
      return $response;
    }

    return NULL;
  }

  private function isBestFitImage($currentImage, $bestFitImage) {
    // The current image is the bestFit, when there the image is smaller than
    // the threshold and larger than the current bestFit.
    if ($this->localFileThreshold) {
      return ($currentImage['size'] < $this->localFileThreshold && $currentImage['size'] > $bestFitImage['size']);
    }
    // When no threshold was set, use the largest one.
    else {
      return ($currentImage['size'] > $bestFitImage['size']);
    }
  }
}
