<?php

interface FotowebGuzzleConfiguratorInterface {

  /**
   * Returns the Guzzle client configuration array.
   *
   * @param $server
   *  Provided Fotoweb server.
   * @param $token
   *  Provided Fotoweb token.
   *
   * @return array
   *  Guzzle initial client configuration.
   */
  public static function getConfiguration($server, $token);

}
