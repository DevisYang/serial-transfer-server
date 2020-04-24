$(function(){
	var serial = CreateSerialHandler("ws://127.0.0.1:9527", onSockMessage, onSockAlert);
	var serials = [];
	$("#serial_name_reflush").click(function(){
		serial.EnumSerial(function(data){
			console.log("获取串口返回: ", data);
			if(data && data.success) {
				
				serials = [];
				for(var i = 0; i < data.serials.length; i++) {
					var o = {};
					o.id=data.serials[i];
					o.text=data.serials[i];
					serials.push(o);
				}
				console.log("加载串口列表：", serials);
				$("#serial_name_id").combobox("loadData", serials);
			}
			$("#message_view").html(data.message);
		});
	});

	$("#open_btn").click(function(){
		serial.Open($("#serial_name_id").combobox('getValue'), parseInt($("#serial_baud_id").combobox('getValue')), function(data){
			$("#message_view").html(data.message);
		});
	});

	$("#close_btn").click(function(){
		serial.Close(function(data) {
			$("#message_view").html(data.message);
		});
	});

	$("#send_textarea").keydown(function(event) {  
        if (event.keyCode == 13) { 
            // 处理数据发送
            var sData = $("#send_textarea").val();
            serial.Send(sData, function(data) {
				$("#message_view").html(data.message);
            });

            $("#send_textarea").val("");
            return false;
        }
        return true;
    });
});

function onSockMessage(data) {
	var e = $("#recv_textarea");
	var message = e.val();
	message = message + dateFormat("<HH:MM:SS>", new Date()) + data + "\n";
	e.val(message);
	var scrollTop = e[0].scrollHeight;
	e.scrollTop(scrollTop);
}

function onSockAlert(message) {
	
}

function dateFormat(fmt, date) {
    let ret;
    const opt = {
        "Y+": date.getFullYear().toString(),        // 年
        "m+": (date.getMonth() + 1).toString(),     // 月
        "d+": date.getDate().toString(),            // 日
        "H+": date.getHours().toString(),           // 时
        "M+": date.getMinutes().toString(),         // 分
        "S+": date.getSeconds().toString()          // 秒
        // 有其他格式化字符需求可以继续添加，必须转化成字符串
    };
    for (let k in opt) {
        ret = new RegExp("(" + k + ")").exec(fmt);
        if (ret) {
            fmt = fmt.replace(ret[1], (ret[1].length == 1) ? (opt[k]) : (opt[k].padStart(ret[1].length, "0")))
        };
    };
    return fmt;
}
