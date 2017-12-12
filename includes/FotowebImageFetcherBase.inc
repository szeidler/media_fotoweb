<?php

use Fotoweb\FotowebClient;

/**
 * Base class for fetching fotoweb images.
 */
abstract class FotowebImageFetcherBase implements FotowebImageFetcherInterface {

  protected $client;

  protected $resourceUrl;

  protected $localPath;

  /**
   * FotowebImageFetcherBase constructor.
   *
   * @param string $resourceUrl
   * @param string $localPath
   */
  public function __construct($resourceUrl, $localPath) {
    $this->client = FotowebClientFactory::create();
    $this->resourceUrl = $resourceUrl;
    $this->localPath = $localPath;
  }

  /**
   * @return FotowebClient
   */
  public function getClient() {
    return $this->client;
  }

  /**
   * @return mixed
   */
  public function getResourceUrl() {
    return $this->resourceUrl;
  }

  /**
   * @return string
   */
  public function getLocalPath() {
    return $this->localPath;
  }

  /**
   * @inheritdoc
   */
  public function store($url) {
    if (empty($url)) return FALSE;

    $response = $this->getClient()->getRendition($url);

    if ($response && $response->getStatusCode() == 200) {
      $this->stream_content = (string) $response->getBody();
      if (!file_unmanaged_save_data($response->getBody(), $this->localPath, FILE_EXISTS_REPLACE)) {
        return FALSE;
      }
    }

    return TRUE;
  }

}
