<?php

/**
 * Fetcher for Original Images.
 */
class FotowebOriginalImageFetcher extends FotowebImageFetcherBase {

  /**
   * @inheritdoc
   */
  public function getImageUrl() {
    try {
      $asset = $this->getClient()->getAsset(['href' => $this->getResourceUrl()]);
      $rendition_resource = $asset->offsetGet('renditions')[0]['href'];
      $rendition_request = $this->getClient()->getRenditionRequest([
        'rendition_service' => variable_get('media_fotoweb_rendition_service', '/fotoweb/services/renditions'),
        'href' => $rendition_resource,
      ]);
      $url = $rendition_request->getHref();
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_CRITICAL);
      return NULL;
    }
    return $url;

  }

}
