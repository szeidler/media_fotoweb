<?php

/**
 * Interface defines the interface for the storing Guzzle configuration.
 */
interface FotowebGuzzleConfiguratorInterface {

  /**
   * Returns the Guzzle client configuration array.
   *
   * @param string $server
   *   Provided Fotoweb server.
   * @param string $token
   *   Provided Fotoweb token.
   *
   * @return array
   *   Guzzle initial client configuration.
   */
  public static function getConfiguration($server, $token);

}
