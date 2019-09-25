external fun puts(str: String): Int

fun main(): String {
    var msg: String = "initial assignment"
    var msgTrue: String = "True"
    var msgFalse: String = "False"
    msg = if (!(7>5)) msgTrue else msgFalse
    puts(msg)
    return msg
}