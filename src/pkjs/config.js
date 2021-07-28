module.exports = [
  {
    "type": "heading",
    "defaultValue": "Watchface Configuration"
  },
  {
    "type": "section",
    "items": [
      {
        "type": "select",
        "messageKey": "Language",
        "id":"Language",
        "defaultValue": "1",
        "label": "Language",
        "options": [
          {
            "label": "Swedish",
            "value": "1"
          },
          {
            "label": "English",
            "value": "2"
          }
        ]
      }
    ]
  },
  {
    "type": "submit",
    "defaultValue": "Save Settings"
  }
];