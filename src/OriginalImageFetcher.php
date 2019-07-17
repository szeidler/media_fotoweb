<?php

namespace Drupal\media_fotoweb;

class OriginalImageFetcher {

  /**
   * @var \Drupal\media_fotoweb\FotowebClient
   */
  protected $client;

  public function __construct(FotowebClient $client) {
    $this->client = $client;
  }

  /**
   * Returns the original image for a given resource URL.
   *
   * @param $resourceUrl
   *   The given resource URL.
   *
   * @return string|null
   *   The image file contents or NULL on failure.
   */
  public function getImageByResourceUrl($resourceUrl) {
    try {
      $asset = $this->client->getAsset(['href' => $resourceUrl]);
      $rendition_resource = $asset->offsetGet('renditions')[0]['href'];
      $rendition_request = $this->client->client->getRenditionRequest([
        'rendition_service' => $this->client->getRenditionService(),
        'href' => $rendition_resource,
      ]);
      $url = $rendition_request->getHref();
    } catch (\Exception $e) {
      return NULL;
    }
    $response = $this->client->client->getRendition($url);
    $body = (string) $response->getBody();
    return $body;


  }

}
