<?php

require_once 'FotowebTestWrapper.php';

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
   */
  public function testInvalidToken() {
    $this->fotowebBase->setFullApiToken('xxx');
    $response = $this->fotowebBase->authenticate();
    $this->assertNotEquals(200, $response->getStatusCode(), 'Response was 200, although we used an invalid token.');
  }
}
