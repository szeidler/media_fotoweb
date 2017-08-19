<?php

use PHPUnit\Framework\TestCase;

require_once 'includes/FotowebAPI/FotowebBase.inc';

/**
 * Wrapper for ArchiveAgent related tests.
 */
abstract class FotowebArchiveAgentTestWrapper extends TestCase {

  protected $fotowebBase;

  /**
   * Sets up the fixture.
   */
  public function setUp() {
    parent::setUp();

    $server = getenv('FOTOWEB_SERVER');
    $username = getenv('FOTOWEB_API_USERNAME');
    $encryptionSecret = getenv('FOTOWEB_ENCRYPTION_SECRET');
    $guzzleConfigurator = new FotowebGuzzleTestConfigurator();
    $this->fotowebBase = new FotowebArchiveAgentBase($server, $username, $encryptionSecret, $guzzleConfigurator);
  }

}
