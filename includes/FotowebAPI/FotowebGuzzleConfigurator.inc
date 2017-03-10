<?php

/**
 * Class FotowebGuzzleConfigurator
 *
 * Handles the Guzzle client configuration for the drupal application.
 *
 */
class FotowebGuzzleConfigurator implements FotowebGuzzleConfiguratorInterface {

  /**
   * @inheritdoc
   */
  public static function getConfiguration($server, $token) {
    $configuration = array(
      'base_uri' => $server,
      'allow_redirects' => FALSE,
      'headers' => ['FWAPITOKEN' => $token],
    );
    drupal_alter('media_fotoweb_guzzle_configuration', $configuration);
    return $configuration;
  }

}
