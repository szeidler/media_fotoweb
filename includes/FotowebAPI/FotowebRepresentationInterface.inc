<?php

/**
 * Interface for implementing all kinds of Fotoweb representations.
 */
interface FotowebRepresentationInterface {

  /**
   * Initiates a HTTP request using Guzzle.
   *
   * @param $resourceUrl
   *   Fotoweb Resource URL to HTTP request.
   *
   * @return mixed|\Psr\Http\Message\ResponseInterface
   * HTTP response from the initiated request.
   */
  public function initiateRequest($resourceUrl);

}
