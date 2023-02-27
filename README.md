# ELOcalculator
ELO calculator using csv files for data.

Currently using 2 csv files for data exchange and calculation.

Entity database with 3 columns: ID, Rating, Number of games

Match results with 4 columns: ID, Point, Point, ID

ELO calculations:

points difference factor P:
P = 1 + sum(1/2^n, 1, n) where n is difference between the points scored by the participants.

number of games factor M:
Initial M = 20.0
M = M - 1/n where n is number of games a participant has played before.

prediction score Wp:
Wp = 1 / ( 10^(-k/400) + 1 ) where k is the difference between the participant ratings.

actual score W:
1 for win, 0.5 for draw, 0 for lose.

rating change R:
R = P*M*(W-Wp)

initial ratings of all participants: 1000


