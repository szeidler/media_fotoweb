<?php

/**
 * Class FotowebAsset
 *
 * @see https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/Asset_representation
 */
class FotowebAsset implements FotowebRepresentationInterface {

  protected $fotoweb;
  protected $data;
  protected $localFileMaxSize;

  const headerAccept = 'application/vnd.fotoware.asset+json';

  public function __construct(FotowebBase $fotoweb, $localFileMaxSize = NULL) {
    $this->fotoweb = $fotoweb;
    $this->localFileMaxSize = $localFileMaxSize;
  }


  public function initiateRequest($resourceUrl) {
    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->request($request);
  }

  public function getAssetImageFromResource($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    $this->data = json_decode($response->getBody(TRUE), TRUE);
    $response = $this->getAssetImageFromAsset();
    return $response;
  }

  /**
   * Return the asset image from a asset resource.
   *
   * @return mixed|null|\Psr\Http\Message\ResponseInterface
   */
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

  /**
   * Checks, if the current iterated image, is a better fit, dependent on
   * maximum local file size.
   *
   * @param $currentImage
   * @param $bestFitImage
   *
   * @return bool
   */
  public function isBestFitImage($currentImage, $bestFitImage) {
    // The current image is the bestFit, when there the image is smaller than
    // the threshold and larger than the current bestFit.
    if ($this->localFileMaxSize) {
      return ($currentImage['size'] < $this->localFileMaxSize && $currentImage['size'] > $bestFitImage['size']);
    }
    // When no threshold was set, use the largest one.
    else {
      return ($currentImage['size'] > $bestFitImage['size']);
    }
  }
}
