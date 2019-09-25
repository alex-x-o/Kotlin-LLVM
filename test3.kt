external fun puts(str: Int): Int

fun main(): String {
    var a: Int = 10
    var b: Int = 15
    var max: Int
    if (a > b) {
        max = a
    } else {
        max = b
    }
    puts(max)
    return ""
}