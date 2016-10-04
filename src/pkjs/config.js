module.exports = [
  {
    "type": "heading",
    "defaultValue": "GymFace Configuration"
  },
     {
        "type": "text",
        "defaultValue": "<h6>A white toggle = OFF, an orange toggle = ON</h6>",
      },

  {
    "type": "section",
    "items": [
 	     {
        "type": "heading",
        "defaultValue": "<h5>Display</h5>"
      },
      {
        "type": "toggle",
        "messageKey": "heart",
        "label": "Display Heart Rate",
        "defaultValue": false
      },
		{
        "type": "toggle",
        "messageKey": "invert",
        "label": "Invert Health Info",
        "defaultValue": false
      }
	]		
},
  {
    "type": "section",
    "items": [	
 	  {
        "type": "heading",
        "defaultValue": "<h5>Vibration</h5>"
      },
      {
        "type": "toggle",
        "messageKey": "bluetoothvibe",
        "label": "Bluetooth Vibration",
        "defaultValue": false
      },
	  {
        "type": "toggle",
        "messageKey": "hourlyvibe",
        "label": "Vibrate each hour",
        "defaultValue": false
      }
    ]
  },
  {
        "type": "text",
        "defaultValue": "<h6>This watchface will continue to be free.  If you find it useful, please consider making a <a href='https://www.paypal.me/markchopsreed'>small donation here</a>. Thankyou.</h6>",
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];