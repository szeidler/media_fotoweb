<?php

/**
 * Fetcher for Preview Images.
 */
class FotowebPreviewImageFetcher extends FotowebImageFetcherBase {

  /**
   * @inheritdoc
   */
  public function getImageUrl() {
    $url = '';
    $localFileThreshold = variable_get('media_fotoweb_local_file_size_threshold', 1500);

    try {
      $asset = $this->client->getAsset(['href' => $this->getResourceUrl()]);

      // Invoke the preview negotiator to negotiate the most appropriate rendition.
      $previewNegotiator = new FotowebPreviewNegotiator($asset, $localFileThreshold);
      if ($bestFitImage = $previewNegotiator->getBestFitImagePath()) {
        $url = $bestFitImage['href'];
      }
    } catch (Exception $e) {
      watchdog('media_fotoweb', $e->getMessage(), array(), WATCHDOG_CRITICAL);
      return NULL;
    }
    return $url;
  }

}
