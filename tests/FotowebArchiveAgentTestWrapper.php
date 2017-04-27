<?php

require_once '../../../../../../vendor/autoload.php';
require_once 'includes/FotowebAPI/FotowebBase.inc';

abstract class FotowebArchiveAgentTestWrapper extends PHPUnit\Framework\TestCase {

  protected $fotowebBase;

  public function setUp() {
    parent::setUp();

    $server = getenv('FOTOWEB_SERVER');
    $username = getenv('FOTOWEB_API_USERNAME');
    $encryptionSecret = getenv('FOTOWEB_ENCRYPTION_SECRET');
    $guzzleConfigurator = new FotowebGuzzleTestConfigurator();
    $this->fotowebBase = new FotowebArchiveAgentBase($server, $username, $encryptionSecret, $guzzleConfigurator);
  }

}
