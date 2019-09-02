val x = 5

fun add(x: Int, y: Int): Int = x + y

external fun puts(str: String): Int

fun main(): String {
    add(5, 10)
    return "Hello World"
}