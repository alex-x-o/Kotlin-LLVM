
fun main(): String {
    var a: Int = 10
    var b: Int = 15
    var max: Int
    if (a > b) {
        max = a
    } else {
        max = b
        a = 12
    }
    println(max)
    println(a)
    return ""
}