<?php

use PHPUnit\Framework\TestCase;

require_once 'includes/FotowebAPI/FotowebBase.inc';

/**
 * Wrapper for RestAPI related tests.
 */
class FotowebTestWrapper extends Testcase {

  protected $fotowebBase;

  /**
   * Sets up the fixture.
   */
  public function setUp() {
    parent::setUp();
    $this->fotowebBase = $this->getFotowebBase();
  }

  /**
   * Initializes the FotowebBase object.
   *
   * @return FotowebBase
   *   FotowebBase object.
   */
  public function getFotowebBase() {
    $server = getenv('FOTOWEB_SERVER');
    $fullApiKey = getenv('FOTOWEB_FULLAPI_KEY');
    $guzzleConfigurator = new FotowebGuzzleTestConfigurator();
    return new FotowebBase($server, $fullApiKey, $guzzleConfigurator);
  }

}
