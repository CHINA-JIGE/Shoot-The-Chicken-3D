var WebBlender = function($) {
  var messageHandlers = [];
  var timeoutMilliseconds = 30000;
  var loadingTimeout;

  /**
      Processes post messages.
      @private
    */
  var processMessage = function(event) {
    var eventToProcess = event.originalEvent || event;
    if (eventToProcess && eventToProcess.data && JSON.parse) {
      var parsed;
      try {
        parsed = JSON.parse(eventToProcess.data);
        if (parsed.message === 'ready') {
          clearTimeout(loadingTimeout);
        }
      } catch (e) {
        // failed to parse message
      }
    }

    for (var i = 0; i < messageHandlers.length; i++) {
      messageHandlers[i](eventToProcess);
    }
  };

  $(window).on("message", processMessage);

  var parentElement;
  var htmlFetched = false;

  var getCliBlendUrl = function(environment, client, flight, deviceFamily) {
    var env = environment && environment.toLowerCase && environment.toLowerCase() || '';
    var format = 'https://{host}/{client}{deviceFamily}{flight}';
    var host;
    switch (env) {
      case "dev":
        host = 'cliblends.dev.microsoft.com';
        break;
      case "int":
        host = 'uniblends.www.microsoft.com/int';
        break;
      case "ppe":
        host = 'uniblends.www.microsoft.com/ppe';
        break;
      case "prod":
      default:
        host = 'www.microsoft.com/uniblends';
        break;
    }

    var url = format
      .replace('{host}', host)
      .replace('{client}', client && $.trim(client) ? '?client=' + client : '?client=')
      .replace('{deviceFamily}', deviceFamily && $.trim(deviceFamily) ? '&deviceFamily=' + $.trim(deviceFamily) : '')
      .replace('{flight}', flight && $.trim(flight) ? '&setflight=' + $.trim(flight) : '');

    return url;
  }

  var getConfigUrl = function(environment, client, flight) {
    var env = environment && environment.toLowerCase && environment.toLowerCase() || '';
    var format = 'https://{host}/webblend/api/config{client}{flight}';
    var host;

    switch (env) {
      case 'dev':
        host = 'oneblend.dev.microsoft.com';
        break;
      case 'int':
        host = 'unistoreblends-int.www.microsoft.com';
        break;
      case 'ppe':
        host = 'unistoreblends-ppe.www.microsoft.com';
        break;
        //case 'prod':
      default:
        host = 'www.microsoft.com';
        break;
    }
    var url = format.replace('{host}', host)
      .replace('{client}', client && $.trim(client) ? '?client=' + client : '')
      .replace('{flight}', flight && $.trim(flight) ? (client && $.trim(client) ? '&setflight=' : '?setflight=') + flight : '');

    return url;
  }

  /**
      Primarily used from tests so that each test can reset the flag and re-fetch html.
      @public
      @method resetHtmlFetched
      @for WebBlender
    */
  var resetHtmlFetched = function(htmlFetch) {
    htmlFetched = htmlFetch;
  };

  /**
      Generates the iframe that will host the blend.
      @private
    */
  var getBlendFrame = function(frameContainer, dimensions) {
    var frameId = 'wb_auto_blend_container';
    var frame = $('#' + frameId);
    var width = dimensions && dimensions.width || '456px';
    var height = dimensions && dimensions.height || '560px';

    if (frame.length) {
      frame.css({
        height: height,
        width: width
      });
    } else {
      frame = $('<iframe />', {
        id: frameId,
        name: frameId,
        src: '',
        style: 'width:{width}; height:{height}; position:relative; top:0; left:0; border:0; outline:none; display:block'.replace('{width}', width).replace('{height}', height)
      });

      frame.appendTo(frameContainer);
    }
    return frame;
  };

  /**
      Checks for flight.
      @private
    */
  var isFlightEnabled = function(flightSetByClient, flightToVerify) {
    var isEnabled = (document && document.cookie && document.cookie.indexOf(flightToVerify) > -1);
    if (!isEnabled) {
      var flights = flightSetByClient && flightSetByClient.split(',') || [];
      for (var i = 0; !isEnabled && i < flights.length; i++) {
        if (flights[i] === flightToVerify) {
          isEnabled = true;
        }
      }
    }
    return isEnabled;
  };

  /**
      Loads the blend.
      @private
    */
  var openBlend = function(postBody, parentElementId, environment, flight, clientType, hash, iframeOptions, deviceFamily) {
    if (!htmlFetched) {
      parentElement = $('#' + parentElementId);

      var start = new Date().getTime();
      var blendContainer = getBlendFrame(parentElement, iframeOptions);

      // old way - create a form, and post form using startData as body.
      // new emberCli way - load blend directly in blend frame. Then send message with startData.

      // Two blender.js partners:
      //  1. AMC - no flight data available.  Will always use default - no emberCli. Unless AMC can flight on their side.
      //  2. For Music/Video, flights should be available.

      var frameId = 'wb_auto_blend_container';

      // For emberCli, need to put postBody fields inside startdata - along with some other config elements
      postBody.Client = clientType;
      postBody.configUrl = getConfigUrl(environment, clientType, flight);

      // selApps sets which blend will load.  Needs to be in the form of: redeem.enterCode
      // which is the route name used in app/routes/index.js transitionTo method.
      // The 'hash' param passed to this openBlend method is in the form of: #/redeem/enterCode
      // so we need to remove the first 2 characters and replace any '/' with '.'.

      // In the future, we may support partners passing some context in this URL, such as PI ID.
      // If this is the case, this simple URL conversion might need to be updated.
      var selApps = hash.substr(2).replace('/', '.');
      postBody.selApps = selApps;

      var iFrameSrc = getCliBlendUrl(environment, clientType, flight, deviceFamily);

      $('#' + frameId).attr('src', iFrameSrc);
      $('#' + frameId).one('load', function() {
        var blendFrame = document.getElementById(frameId);
        blendFrame.contentWindow.postMessage(JSON.stringify({
          startdata: postBody
        }), '*');
      });

      var stop = new Date().getTime();
      var span = stop - start;
      window.postMessage(JSON.stringify({
        message: 'status',
        data: 'blenderSDK: Loaded in ' + span + 'ms'
      }), '*');
    }
  };

  /**
      Updates post body for style.
      @private
    */
  var updateStyle = function(postBody, style) {
    if (style) {
      postBody['Layout'] = style.layout || '';
      postBody['CssOverride'] = style.cssOverride || '';
    }
  };

  /**
      Loads the Html for the purchase blend into a div child of the provided element.
      @public
      @method loadSingleItemPurchaseHtml
      @for WebBlender
      @param {availabilityId} availability id of product to add to new order (required)
      @param {productId} product id of product to add to new order (required)
      @param {skuId} sku id of product to add to new order (required)
      @param {options} campaign options (optional)
      @param {auth} authentication (required)
      @param {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      @param {environment} see resolveEnvironment for options (required)
      @param {flight} see wiki for options (optional)
      @param {clientType} see clientTypes (required)
      @param {culture} UI culture (required)
      @param {market} service market (required)
      @param {cv} correlation-vector (required)
      @param {identityType} auth identityType (optional)
      @param {identityValue} auth identityValue (optional)
      @param {mediaOptions} media-specific options (optional)
      @param {iframeOptions} object specifying desired width and height overrides of iframe (optional)
      @param {purchaseServiceVersion} string specifying desired version of purchase service to use. Will default if not supplied to oldest supported version (optional)
      @param {style} layout and css override (optional)
      @param {quantity} quantity of the item to add to new order (optional)
      @param {deviceFamily} deviceFamily in which Blends will be loaded. (e.g Windows.Xbox or Windows.Desktop) see the wiki for more details (optional)
      @param {testClient} only passed if its Quality Store pretending to be a client (e.g QualityStore ) (optional)
    */
  var loadSingleItemPurchaseHtml = function(availabilityId, productId, skuId, options, auth, parentElementId, environment, flight, clientType, culture, market, cv, identityType, identityValue, mediaOptions, iframeOptions, purchaseServiceVersion, style, quantity, deviceFamily, testClient) {
    var postBody = {
      "AvailabilityId": availabilityId,
      "ProductId": productId,
      "SkuId": skuId,
      "Quantity": quantity || 1,
      "Auth": auth,
      "Culture": culture,
      "Market": market,
      "CV": cv,
      "IdentityType": identityType,
      "IdentityValue": identityValue,
      "PurchaseServiceVersion": purchaseServiceVersion,
      "TestClient": testClient || "", // this will only be used for filtering BI reports
      // For WebStore, all exclusivity parameters should be set to PUBLIC
      "DeviceContext": "moId=PUBLIC&oemId=PUBLIC&scmId=PUBLIC",
    };

    if (options) {
      postBody["CampaignId"] = options.campaignId || "";
      postBody["OptionalCampaignId"] = options.optionalCampaignId || "";
      postBody["OfferType"] = options.offerType || "";
    }

    if (mediaOptions) {
      postBody["ProductType"] = mediaOptions.productType || "";
      postBody["TransactionType"] = mediaOptions.transactionType || "buy";
      postBody["Title"] = mediaOptions.title || "";
      postBody["TitleNo"] = mediaOptions.titleNo || "";
      postBody["SubTitle1"] = mediaOptions.subTitle1 || "";
      postBody["SubTitle2"] = mediaOptions.subTitle2 || "";
      postBody["ExpirationBeforePlayInHours"] = mediaOptions.expirationBeforePlayInHours || "";
      postBody["ExpirationAfterPlayInHours"] = mediaOptions.expirationAfterPlayInHours || "";
    }

    updateStyle(postBody, style);

    openBlend(postBody, parentElementId, environment, flight, clientType, "#/purchase/confirm", iframeOptions, deviceFamily || "");
  };

  /**
      Loads the Html for the purchase blend into a div child of the provided element.
      @public
      @method loadOrderPurchaseHtml
      @for WebBlender
      @param {orderId} order id (required)
      @param {auth} authentication (required)
      @param {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      @param {environment} see resolveEnvironment for options (required)
      @param {flight} see wiki for options (optional)
      @param {clientType} see clientTypes (required)
      @param {culture} UI culture (required)
      @param {market} service market (required)
      @param {cv} correlation-vector (required)
      @param {iframeOptions} object specifying desired width and height overrides of iframe (optional)
      @param {purchaseServiceVersion} string specifying desired version of purchase service to use. Will default if not supplied to oldest supported version (optional)
      @param {jwt} token for beneficiary relationship with purchaser (optional)
      @param {beneficiaryData} data for beneficiary msa information , includes First name, last name and msa email address (optional)
      @param {style} layout and css override (optional)
      @param {deviceFamily} deviceFamily in which Blends will be loaded. (e.g Windows.Xbox or Windows.Desktop) see the wiki for more details (optional)
      @param {testClient} only passed if its Quality Store pretending to be a client (e.g QualityStore ) (optional)
    */
  var loadOrderPurchaseHtml = function(orderId, auth, parentElementId, environment, flight, clientType, culture, market, cv, iframeOptions, purchaseServiceVersion, jwt, beneficiaryData, style, deviceFamily, testClient) {
    var postBody = {
      "OrderId": orderId,
      "Auth": auth,
      "Culture": culture,
      "Market": market,
      "CV": cv,
      "PurchaseServiceVersion": purchaseServiceVersion,
      "Jwt": jwt,
      // For WebStore, all exclusivity parameters should be set to PUBLIC
      "TestClient": testClient || "", // this will only be used for filtering BI reports
      "DeviceContext": "moId=PUBLIC&oemId=PUBLIC&scmId=PUBLIC"
    };
    if (beneficiaryData) {
      postBody["BeneficiaryEmail"] = beneficiaryData.email;
      postBody["BeneficiaryFirstName"] = beneficiaryData.firstName || "";
      postBody["BeneficiaryLastName"] = beneficiaryData.lastName || "";
    }

    updateStyle(postBody, style);

    openBlend(postBody, parentElementId, environment, flight, clientType, "#/purchase/confirm", iframeOptions, deviceFamily || "");
  };


  /** This API will be deprecated soon. Use addPaymentInstrument.
      Loads the Html for the Add PI blend into a div child of the provided element.
      @public
      @method loadAddPaymentInstrumentHtml
      @for WebBlender
      @param {auth} authentication (required)
      @param {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      @param {environment} see resolveEnvironment for options (required)
      @param {flight} see wiki for options (optional)
      @param {clientType} see clientTypes (required)
      @param {culture} UI culture (required)
      @param {market} service market (required)
      @param {cv} correlation-vector (required)
      @param {iframeOptions} object specifying desired width and height overrides of iframe (optional)
      @param {style} layout and css override (optional)
*/
  var loadAddPaymentInstrumentHtml = function(auth, parentElementId, environment, flight, clientType, culture, market, cv, iframeOptions, style) {
    var postBody = {
      "Auth": auth,
      "Culture": culture,
      "Market": market,
      "CV": cv,
      "context": "primaryPi"
    };

    updateStyle(postBody, style);

    openBlend(postBody, parentElementId, environment, flight, clientType, "#/paymentAndBilling/choosePaymentMethodFamily", iframeOptions);
  };


  /** This API will be deprecated soon, use editPaymentInstrument.
      Loads the Html for the edit payment instrument blend into a div child of the provided element.
      @public
      @method loadEditPaymentInstrumentHtml
      @for WebBlender
      @param {auth} authentication (required)
      @param {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      @param {environment} see resolveEnvironment for options (required)
      @param {flight} see wiki for options (optional)
      @param {clientType} see clientTypes (required)
      @param {culture} UI culture (required)
      @param {market} service market (required)
      @param {cv} correlation-vector (required)
      @param {paymentInstrumentId} payment instrument id (required)
      @param {iframeOptions} object specifying whether to use iframe, and if so, desired width and height of iframe (optional)
      @param {style} layout and css override (optional)
    */
  var loadEditPaymentInstrumentHtml = function(auth, parentElementId, environment, flight, clientType, culture, market, cv, paymentInstrumentId, iframeOptions, style) {
    var postBody = {
      "Auth": auth,
      "Culture": culture,
      "Market": market,
      "CV": cv,
      "PaymentInstrumentId": paymentInstrumentId
    };

    updateStyle(postBody, style);

    openBlend(postBody, parentElementId, environment, flight, clientType, "#/paymentAndBilling/editPaymentInstrument", iframeOptions);
  };

  /**
      Loads the Html for the Add PI blend into a div child of the provided element.
      @public
      @method addPaymentInstrument
      @param {inputData} it is an object with all required properties
        inputData = {
            "authCT": authCT,
            "authXToken": authXToken,
            "parentElementId": parentElementId,
            "environment": environment,
            "flight": flight,
            "clientType": clientType,         
            "culture": culture,           
            "market": market,
            "cv": cv,
            "iframeOptions": iframeOptions,
            "style": style,
            "deviceFamily": deviceFamily,
            "paymentMethodFamily": paymentMethodFamily,
            "paymentMethodType": paymentMethodType
        }
      {authCT } RPS compact ticket (either authCT or authXToken is required)
      {authXToken} xToken (either authCT or authXToken is required)
      {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      {environment} see resolveEnvironment for options (required)
      {flight} see wiki for options (optional)
      {clientType} see clientTypes (required)
      {culture} UI culture (required)
      {market} service market (required)
      {cv} correlation-vector (required)
      {iframeOptions} object specifying desired width and height overrides of iframe (optional)
      {style} layout and css override (optional)
      {deviceFamily} deviceFamily in which Blends will be loaded. see the wiki for more details (required)
      {paymentMethodFamily} paymentMethodFamily of payment Instrument. see the wiki for more details (required)
      {paymentMethodType} paymentMethodFamily of payment Instrument. see the wiki for more details  (required)
*/
  var addPaymentInstrument = function(inputData) {
    var postBody = {};
    if (inputData) {
      postBody["Auth"] = inputData.authCT;
      postBody["AuthXToken"] = inputData.authXToken;
      postBody["Culture"] = inputData.culture || "";
      postBody["Market"] = inputData.market || "";
      postBody["CV"] = inputData.cv || "";
      postBody["context"] = "primaryPi";
      postBody["PaymentMethodFamily"] = inputData.paymentMethodFamily || "";
      postBody["PaymentMethodType"] = inputData.paymentMethodType || "";
      var deviceFamily = inputData.deviceFamily || "web";

      updateStyle(postBody, inputData.style);

      openBlend(postBody, inputData.parentElementId, inputData.environment, inputData.flight, inputData.clientType, "#/paymentAndBilling/choosePaymentMethodFamily", inputData.iframeOptions, deviceFamily);
    }
  };



  /**
      Loads the Html for the Edit PI blend into a div child of the provided element.
      @public
      @method editPaymentInstrument
      @param {inputData} it is an object with all required properties
        inputData = {
            "authCT": authCT,
            "authXToken": authXToken,
            "parentElementId": parentElementId,
            "environment": environment,
            "flight": flight,
            "clientType": clientType,         
            "culture": culture,           
            "market": market,
            "cv": cv,
            "iframeOptions": iframeOptions,
            "style": style
        }
      {authCT } RPS compact ticket (either authCT or authXToken is required)
      {authXToken} xToken (either authCT or authXToken is required)
      {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
      {environment} see resolveEnvironment for options (required)
      {flight} see wiki for options (optional)
      {clientType} see clientTypes (required)
      {culture} UI culture (required)
      {market} service market (required)
      {cv} correlation-vector (required)
      {iframeOptions} object specifying desired width and height overrides of iframe (optional)
      {style} layout and css override (optional)
      {deviceFamily} deviceFamily in which Blends will be loaded. see the wiki for more details (required)
      {paymentInstrumentId} The ID of the payment instrument being edited. (required) 
*/
  var editPaymentInstrument = function(inputData) {
    var postBody = {};
    if (inputData) {
      postBody["Auth"] = inputData.authCT;
      postBody["AuthXToken"] = inputData.authXToken;
      postBody["Culture"] = inputData.culture || "";
      postBody["Market"] = inputData.market || "";
      postBody["CV"] = inputData.cv || "";
      postBody["PaymentInstrumentId"] = inputData.paymentInstrumentId;
      var deviceFamily = inputData.deviceFamily || "web";

      updateStyle(postBody, inputData.style);

      openBlend(postBody, inputData.parentElementId, inputData.environment, inputData.flight, inputData.clientType, "#/paymentAndBilling/editPaymentInstrument", inputData.iframeOptions, deviceFamily);
    }
  };

  /**
     Loads the Html for the Redeem Token blend into a div child of the provided element.
     @public
     @method loadRedeemHtml
     @for WebBlender
     @param {tokenString} Token string. If non-empty, blend will skip token input page, and go directly to confirm page. (optional)
     @param {auth} authentication (required)
     @param {parentElementId} DOM element (e.g. div, span) into which Blends will be loaded (required)
     @param {environment} see resolveEnvironment for options (required)
     @param {flight} see wiki for options (optional)
     @param {clientType} see clientTypes (required)
     @param {culture} UI culture (required)
     @param {market} service market (required)
     @param {cv} correlation-vector (required)
     @param {iframeOptions} object specifying desired width and height overrides of iframe (optional)
     @param {style} layout and css override (optional)
   */
  var loadRedeemHtml = function(tokenString, auth, parentElementId, environment, flight, clientType, culture, market, cv, iframeOptions, style) {
    var postBody = {
      "tokenString": tokenString,
      "Auth": auth,
      "Culture": culture,
      "Market": market,
      "CV": cv,
      "PurchaseServiceVersion": "6"
    }

    updateStyle(postBody, style);

    // Temporarily ignore flight param. Passing a flight here doesn't work in prod, and somehow nullifies PurchaseServiceVersion:6.
    // Will re-add once AMC stops passing in purchasev6:1 flight.
    openBlend(postBody, parentElementId, environment, null, clientType, "#/redeem/enterCode", iframeOptions);
  };

  /**
      Subscribes to the blend post messages.
      @public
      @method registerMessageHandler
      @for WebBlender
      @param {newHandler} subscriber to add
    */
  var registerMessageHandler = function(newHandler) {
    for (var i = 0; i < messageHandlers.length; i++) {
      if ('' + messageHandlers[i] === '' + newHandler) {
        // See 5056068
        console.error('Same message handler function already registered: ' + newHandler);
        return;
      }
    }
    messageHandlers.push(newHandler);
  };

  /**
      Unsubscribes from the blend post messages.
      @public
      @method unregisterMessageHandler
      @for WebBlender
      @param {handler} subscriber to remove
    */
  var unregisterMessageHandler = function(handler) {
    for (var i = 0; i < messageHandlers.length; i++) {
      if ('' + messageHandlers[i] === '' + handler) {
        messageHandlers.pop(handler);
      }
    }
  };

  /**
      Mapping of client types.
      @public
      @object clientTypes
      @for WebBlender
    */
  var clientTypes = {
    UniversalWebStore: "UniversalWebStore",
    MusicVideoReading: "MusicVideoReading",
    EnterpriseStore: "EnterpriseStore",
    AccountMicrosoftCom: "AccountMicrosoftCom"
  };

  return {
    loadSingleItemPurchaseHtml: loadSingleItemPurchaseHtml,
    loadOrderPurchaseHtml: loadOrderPurchaseHtml,
    loadRedeemHtml: loadRedeemHtml,
    loadAddPaymentInstrumentHtml: loadAddPaymentInstrumentHtml,
    loadEditPaymentInstrumentHtml: loadEditPaymentInstrumentHtml,
    registerMessageHandler: registerMessageHandler,
    unregisterMessageHandler: unregisterMessageHandler,
    clientTypes: clientTypes,
    resetHtmlFetched: resetHtmlFetched,
    addPaymentInstrument: addPaymentInstrument,
    editPaymentInstrument: editPaymentInstrument

  };
}(jQuery);