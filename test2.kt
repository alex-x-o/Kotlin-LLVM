
external fun puts(str: String): Int

fun main(): String {
    var i: Int = 10
    var j: Int = 10
    var msgTrue: String = "True"
    var msgFalse: String = "False"
    var msg: String = if (i <= j) msgTrue else msgFalse
    puts(msg)
    return msg
}