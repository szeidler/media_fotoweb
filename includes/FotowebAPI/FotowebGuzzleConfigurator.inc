<?php

use GuzzleHttp\HandlerStack;
use GuzzleHttp\Middleware;

/**
 * FotowebGuzzleConfigurator handles the Guzzle client configuration.
 */
class FotowebGuzzleConfigurator implements FotowebGuzzleConfiguratorInterface {

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
    $configuration = self::addRetryHandler($configuration);
    drupal_alter('media_fotoweb_guzzle_configuration', $configuration);
    return $configuration;
  }

  /**
   * Adds a retry handler stack, that repeats the request on failure.
   *
   * @see http://dawehner.github.io/php,/guzzle/2017/05/19/guzzle-retry.html
   *
   * @param array $configuration
   *   Guzzle initial client configuration.
   *
   * @return mixed
   *   Guzzle initial client configuration.
   */
  protected static function addRetryHandler(array $configuration) {
    $handler_stack = HandlerStack::create();
    $handler_stack->push(Middleware::retry(function ($retry, $request, $value, $reason) {
      // If we have a value already, we should be able to proceed quickly.
      if ($value !== NULL) {
        return FALSE;
      }

      // Reject after 10 retries.
      return $retry < 10;
    }));

    $configuration['handler'] = $handler_stack;
    return $configuration;
  }

}
