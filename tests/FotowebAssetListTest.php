<?php

require_once 'FotowebTestWrapper.php';

/**
 * Test to fetch an assetlist from an archive.
 */
class FotowebAssetListTest extends FotowebTestWrapper {

  protected $fotowebAssetList;

  /**
   * Sets up the fixture.
   */
  public function setUp() {
    parent::setUp();
    $this->fotowebAssetList = new FotowebAssetList($this->fotowebBase);
  }

  /**
   * Tests to fetch an assetlist from a fixed asset resource.
   */
  public function testFetchSpecificAssetList() {
    $response = $this->fotowebAssetList->initiateRequest('fotoweb/data/a/5000.fMRICEsZB2hL-r4J5Efx3Q/');
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');

    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertArrayHasKey('data', $data, 'Response misses data property.');
    $this->assertGreaterThan(0, count($data['data']), 'Response misses data items.');
  }

  /**
   * Tests to fetch an assetlist from the first available archive.
   */
  public function testFetchAssetListFromArchiveTraversal() {
    $fotowebArchive = new FotowebArchives($this->fotowebBase);
    $response = $fotowebArchive->initiateRequest('fotoweb/me/archives/');
    $data = json_decode($response->getBody(TRUE), TRUE);
    $this->assertGreaterThan(0, count($data['data']), 'Response misses data items.');

    $assetResource = $data['data'][0]['data'];

    $response = $this->fotowebAssetList->initiateRequest($assetResource);
    $this->assertEquals(200, $response->getStatusCode(), 'Response was not 200.');
    $this->assertNotEmpty((string) $response->getBody(), 'Response body was empty.');
  }

}
