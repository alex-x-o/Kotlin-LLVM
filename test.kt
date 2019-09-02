
external fun puts(str: String): Int

fun add(x: Int, y: Int): Int = x + y


fun main(): String {
    var t: Int = add(5, 8)
    var msg: String = "Hello"
    msg = "Hello, World"
    puts(msg)
    return msg
}