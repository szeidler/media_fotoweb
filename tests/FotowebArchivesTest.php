<?php

require_once 'FotowebTestWrapper.php';

class FotowebArchivesTest extends FotowebTestWrapper {

  protected $fotowebArchives;

  public function setUp() {
    parent::setUp();
    $this->fotowebArchives = new FotowebArchives($this->fotowebBase);
  }

  public function testFetchArchives() {
    $response = $this->fotowebArchives->initiateRequest('fotoweb/me/archives/');
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');

    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertArrayHasKey('data', $data, 'Response misses data property.');
    $this->assertGreaterThan(0, count($data['data']), 'Response misses data items.');
  }
}
