<?php

namespace Drupal\media_fotoweb;

use Drupal\Core\Config\ConfigFactoryInterface;

/**
 * Factory that builds the FotowebClient.
 */
class FotowebClientFactory {

  /**
   * Creates the FotowebClient from configuration values.
   *
   * @param \Drupal\Core\Config\ConfigFactoryInterface $config_factory
   *   The config factory.
   *
   * @return \Drupal\media_fotoweb\FotowebClient
   *   The Fotoweb Client.
   */
  public static function create(ConfigFactoryInterface $config_factory) {
    $config = $config_factory->get('media_fotoweb.settings');
    $client_configuration = [
      'baseUrl' => $config->get('server'),
      'apiToken' => $config->get('full_api_key'),
      'client_config' => ['allow_redirects' => FALSE],
    ];

    $client = new FotowebClient($client_configuration);

    $rendition_service = $config->get('rendition_service');
    if (!empty($rendition_service)) {
      $client->setRenditionService($rendition_service);
    }

    return $client;
  }

}
