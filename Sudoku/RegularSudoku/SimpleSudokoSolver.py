board = [
    [7,8,0,4,0,0,1,2,0],
    [6,0,0,0,7,5,0,0,9],
    [0,0,0,6,0,1,0,7,8],
    [0,0,7,0,4,0,2,6,0],
    [0,0,1,0,5,0,9,3,0],
    [9,0,4,0,6,0,0,0,5],
    [0,7,0,3,0,0,0,1,2],
    [1,2,0,0,0,7,4,0,0],
    [0,4,9,2,0,6,0,0,7]
]

def print_board(board):
    for x in range(len(board)):
        if x % 3 == 0 and x != 0:
            print("- - - - - - - - - - - -")
        for y in range(len(board[0])):
            if y % 3 == 0 and y !=  0:
                print(" | ", end="")
            if y < 8:
                print(str(board[x][y]) + " ", end="")
            else:
                print(board[x][y])

empty_coord = []
def isEmpty(board, coord_list):     #to get a list of tuples of coord with 0 in it
    for x in range(len(board)):
        for y in range(len(board[0])):
            if board[x][y] == 0:
                coord_list += [(x, y)]      # [(row, column)]


def isCorrect(board, num, coord):               # check if the given number is suitable to be put in
    for x in range(len(board[coord[0]])):
        if num == board[coord[0]][x]:           # check the row of the board
            return False

        if num == board[x][coord[1]]:           # check the column of the board
            return False

    for x in range(coord[0]*3, coord[0]*3 + 3):         # check the box of the board
        for y in range(coord[1]*3, coord[1]*3 + 3):
            if num == board[x][y]:
                return False

    return True






print_board(board)
isEmpty(board, empty_coord) # populate empty_coord
print(empty_coord)
print(isCorrect(board, 6, empty_coord[0]))