//balance_car.js
const app = getApp()

const bc_param_list = ["平衡PID", "速度PID"]

const bc_paam_table = [["平衡-P", "平衡-I", "平衡-D",],
                       ["速度-P", "速度-I", "速度-D",],
                       ["速度",   "待定",   "待定",]]



Component({
  data: {
    bc_param: bc_param_list,
  },

  printLog:function(log) {
    var logs = this.data.logs;
    logs.push(log);
    this.setData({log_list: logs.join('\n')})
  },

  bc_param_change(e) {
    const val = e.detail.value
    this.setData({
      bc_param: this.data.bc_param[val],
    })
    this.printLog(this.data.bc_parm[val])
  },

  pageLifetimes: {
    show() {
      if (typeof this.getTabBar === 'function' &&
        this.getTabBar()) {
        this.getTabBar().setData({
          selected: 0
        })
      }
    }
  }
})