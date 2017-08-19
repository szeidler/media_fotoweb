<?php

require_once 'FotowebTestWrapper.php';

/**
 * Test to authenticate with the Fotoweb API
 */
class FotowebBaseTest extends FotowebTestWrapper {

  /**
   * Test the successful server-to-server authentication to the API.
   */
  public function testAuthenticate() {
    $response = $this->fotowebBase->authenticate();
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty($response->getBody(), 'Response body was empty.');
  }

  /**
   * Assure, that an invalid token will not return a 200 response.
   *
   * @expectedException Exception
   * @expectedExceptionCode 403
   */
  public function testInvalidToken() {
    $this->fotowebBase->setFullApiKey('xxx');
    $this->fotowebBase->authenticate();
  }

}
