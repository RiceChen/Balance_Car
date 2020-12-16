Component({
  data: {
    selected: 0,
    color: "#7A7E83",
    selectedColor: "#3cc51f",
    list: [
      {
        "pagePath": "/balance_car/balance_car",
        "iconPath": "/image/balance_car_off.png",
        "selectedIconPath": "/image/balance_car_on.png",
        "text": "平衡车"
      },
      {
        "pagePath": "/ble/ble",
        "iconPath": "/image/bt_off.png",
        "selectedIconPath": "/image/bt_on.png",
        "text": "BLE"
      }
    ]
  },
  attached() {
  },
  methods: {
    switchTab(e) {
      const data = e.currentTarget.dataset
      const url = data.path
      wx.switchTab({url})
      this.setData({
        selected: data.index
      })
    }
  }
})