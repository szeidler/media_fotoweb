<?php

/**
 * Class FotowebGuzzleTestConfigurator
 *
 * Handles the Guzzle client configuration for the modules unit tests.
 *
 */
class FotowebGuzzleTestConfigurator implements FotowebGuzzleConfiguratorInterface {

  /**
   * @inheritdoc
   */
  public static function getConfiguration($server, $token) {
    $configuration = array(
      'base_uri' => $server,
      'allow_redirects' => FALSE,
      'headers' => ['FWAPITOKEN' => $token],
    );
    $proxy = getenv('FOTOWEB_GUZZLE_PROXY');
    if (!empty($proxy)) {
      $configuration['proxy'] = $proxy;
    }
    return $configuration;
  }

}
