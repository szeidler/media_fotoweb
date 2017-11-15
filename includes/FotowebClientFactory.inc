<?php

use Fotoweb\FotowebClient;

class FotowebClientFactory {

  /**
   * @param $baseUrl
   *   BaseUrl of the Fotoweb instance.
   * @param $apiToken
   *   Full API Key of the Fotoweb instance.
   *
   * @return FotowebClient
   *   Client to be used for all API interaction.
   */
  public static function create($baseUrl = NULL, $apiToken = NULL) {
    if (empty($baseUrl)) {
      $baseUrl = variable_get('media_fotoweb_server');
    }
    if (empty($apiToken)) {
      $apiToken = variable_get('media_fotoweb_full_api_key');
    }

    return new FotowebClient([
      'baseUrl' => $baseUrl,
      'apiToken' => $apiToken,
    ]);
  }
}
