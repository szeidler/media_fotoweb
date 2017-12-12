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

    $config = [
      'baseUrl' => $baseUrl,
      'apiToken' => $apiToken,
      'client_config' => ['allow_redirects' => FALSE],
    ];

    drupal_alter('media_fotoweb_guzzle_configuration', $config['client_config']);

    return new FotowebClient($config);
  }
}
