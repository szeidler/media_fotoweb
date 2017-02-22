<?php

/**
 * Class FotowebAssetList
 *
 * @see https://learn.fotoware.com/02_FotoWeb_8.0/Developing_with_the_FotoWeb_API/01_The_FotoWeb_RESTful_API/Asset_list_representation
 */
class FotowebAssetList extends FotowebCollection implements FotowebRepresentationInterface {

  const headerAccept = 'application/vnd.fotoware.assetlist+json';

}
