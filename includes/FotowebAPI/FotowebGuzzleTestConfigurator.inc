<?php

/**
 * Class handles the Guzzle client configuration for the modules unit tests.
 */
class FotowebGuzzleTestConfigurator extends FotowebGuzzleConfigurator {

  /**
   * Returns the Guzzle client configuration array.
   *
   * @inheritdoc
   */
  public static function getConfiguration($server, $token) {
    $configuration = array(
      'base_uri' => $server,
      'allow_redirects' => FALSE,
    );
    if (!empty($token)) {
      $configuration['headers'] = array('FWAPITOKEN' => $token);
    }
    $proxy = getenv('FOTOWEB_GUZZLE_PROXY');
    if (!empty($proxy)) {
      $configuration['proxy'] = $proxy;
    }
    $configuration = self::addRetryHandler($configuration);
    return $configuration;
  }

}
