<?php

/**
 * Class FotowebAsset
 *
 * @see https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/Asset_representation
 */
class FotowebAsset implements FotowebRepresentationInterface {

  protected $fotoweb;
  protected $data;

  const headerAccept = 'application/vnd.fotoware.asset+json';

  public function __construct(FotowebBase $fotoweb) {
    $this->fotoweb = $fotoweb;
  }


  public function initiateRequest($resourceUrl) {
    $this->fotoweb->ensureAuthentication();

    $request = new GuzzleHttp\Psr7\Request('GET', $resourceUrl, [
      'Accept' => static::headerAccept,
    ]);

    return $this->fotoweb->request($request);
  }

  public function getAssetThumbnailFromResource($resourceUrl) {
    $response = $this->initiateRequest($resourceUrl);
    $this->data = drupal_json_decode($response->getBody(TRUE), TRUE);
    $thumbnail = $this->getAssetThumbnailFromAsset();
    return $thumbnail;
  }

  private function getAssetThumbnailFromAsset() {
    // Iterate over previews and use the largest cropped square image.
    if (!empty($this->data['previews'])) {
      $best_fit_image = NULL;
      foreach ($this->data['previews'] as $previewImage) {
        if ($previewImage['square'] && (!$best_fit_image || $previewImage['size'] > $best_fit_image['size'])) {
          $best_fit_image = $previewImage;
        }
      }

      // Request the designated preview image.
      $request = new GuzzleHttp\Psr7\Request('GET', $best_fit_image['href']);
      $response = $this->fotoweb->request($request);
      return $response->getBody();
    }

    return NULL;
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
      $best_fit_image = NULL;
      foreach ($this->data['previews'] as $previewImage) {
        if (!$previewImage['square'] && (!$best_fit_image || $previewImage['size'] > $best_fit_image['size'])) {
          $best_fit_image = $previewImage;
        }
      }

      // Request the designated preview image.
      $request = new GuzzleHttp\Psr7\Request('GET', $best_fit_image['href']);
      $response = $this->fotoweb->request($request);
      return $response;
    }

    return NULL;
  }
}
