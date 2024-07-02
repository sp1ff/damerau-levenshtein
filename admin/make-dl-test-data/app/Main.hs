module Main where

import Paths_make_dl_test_data (version)
import Data.Version (showVersion)


import Data.Char (chr)
import Data.List (intercalate, unfoldr)
import Data.Maybe (catMaybes)
import Data.Time
import Data.Time.Clock.POSIX
import Data.Vector (Vector, (!), fromList)
import System.Console.GetOpt
import System.Environment (getArgs)
import System.Random
import Text.EditDistance
import Text.Printf
import Text.Read (readMaybe)

makeChar :: RandomGen g => g -> (Char, g)
makeChar g
  =
    let (c, h) = uniformR (1, 26) g
    in (chr (c + 64), h)

insertAt :: [Char] -> Int -> Char -> [Char]
insertAt s i c
  =
  let (before, after) = splitAt i s
  in before ++ [c] ++ after

deleteAt :: [Char] -> Int -> [Char]
deleteAt s i
  =
  let (before, after) = splitAt i s
  in before ++ drop 1 after

rewriteAt :: [Char] -> Int -> Char -> [Char]
rewriteAt s i c
  =
  let (before, after) = splitAt i s
  in before ++ [c] ++ drop 1 after

transposeAt :: [Char] -> Int -> [Char]
transposeAt s i
  =
  let (before, after) = splitAt (i + 1) s
  in (init before) ++ [head after] ++ [last before] ++ (tail after)

genString :: RandomGen g => g -> Int -> [Char]
genString g n
 =
  map
    (\n -> chr (n + 64))
    (take n (unfoldr (Just . uniformR (1, 26)) g))

editString1_0 :: RandomGen g => g -> ([Char], g)
editString1_0 g
  =
    let (c, h) = makeChar g
    in ([c], h)

editString1_1 :: RandomGen g => [Char] -> g -> ([Char], g)
editString1_1 s g
  =
    let (c, h) = makeChar g
        (i, j) = uniformR (0 :: Int, 1 :: Int) h
        (e, k) = uniformR (1 :: Int, 3 :: Int) k
    in if e == 1 -- Insert
       then (insertAt s i c, k)
       else if e == 2 -- Delete
            then ([], k)
            else ([c], k)

editString1_2 :: RandomGen g => [Char] -> g -> ([Char], g)
editString1_2 s g
  =
    let n = length s
        (c, h) = makeChar g
        (e, j) = uniformR (1 :: Int, 4 :: Int) h
    in if e == 1 -- Insert
       then let (i, k) = uniformR (0, n) j
            in (insertAt s i c, k)
       else if e == 2 -- Delete
            then let (i, k) = uniformR (0, n - 1) j
                 in (deleteAt s i, k)
            else if e == 3 -- Rewrite
                 then let (i, k) = uniformR (0, n - 1) j
                      in (rewriteAt s i c, k)
                 else -- Transpose
                   let (i, k) = uniformR (0, n - 2) j
                         in (transposeAt s i, k)

editString1 :: RandomGen g => ([Char], g) -> ([Char], g)
editString1 (s, g)
  =
  case (length s) of
    0 -> editString1_0 g
    1 -> editString1_1 s g
    _ -> editString1_2 s g
  
editString :: RandomGen g => g -> [Char] -> Int -> [Char]
editString g s n
  =
   fst
   ((iterate
     editString1
     (s, g)) !! n)

generateTestData :: RandomGen g => g -> Int -> Int -> Int -> [(String, String, Int)]
generateTestData g stringLength numEdits numCases
  =
  take
  numCases
  (unfoldr
  -- g -> (s, t, d)
  (\g ->
      let (n, h) = uniformR (1, numEdits) g
          s = genString h stringLength
          t = editString h s n
          d = restrictedDamerauLevenshteinDistance defaultEditCosts s t
      in Just ((s, t, d), h))
  g)

--------------------------------------------------------------------------------
-- Sum/sigma type describing our program options
data Flag
  = Help
  | NumEdits (Maybe Int)
  | StringLength (Maybe Int)
  | Version
  deriving (Show, Eq)

options =
  [ Option
      ['h']
      ["help"]
      (NoArg Help)
      "Display this usage message on stdout & exit with status zero"
  , Option
    ['e']
    ["num-edits"]
    (ReqArg (\arg -> NumEdits (readMaybe arg :: Maybe Int)) "EDITS")
    "Set the maximum number of edit operations when generating test data"
  , Option
      ['L']
      ["length"]
      (ReqArg (\arg -> StringLength (readMaybe arg :: Maybe Int)) "LEN")
      "Generate test data whose first string is of length LEN"
  , Option
      ['V']
      ["version"]
      (NoArg Version)
      "Print this program's version on stdout & exit with status zero"
  ]

-- Sum/Sigma type enumerating the three things this program can do
data Options
  = ShowHelp
  | ShowVersion
  | GenerateTestData (Int, Int, Int) -- len, edits, cases
  deriving (Show, Eq)

-- Take a list of `Flag`, sift through it for -L; handle the cases of
-- zero occurrences (use default behavior), one (-L given), or > 1
-- (error)(
parseStringLength :: [Flag] -> Int
parseStringLength opts =
  let lengths =
        catMaybes
          (map
             (\opt ->
                (case opt of
                   StringLength x ->
                     case x of
                       Just n -> Just n
                       Nothing -> errorWithoutStackTrace "Bad --length argument"
                   _ -> Nothing))
             opts)
   in case (length lengths) of
        0 -> 5
        1 -> (head lengths)
        _ -> errorWithoutStackTrace "--length may be given zero or one times"

parseNumberOfEdits :: [Flag] -> Int
parseNumberOfEdits opts =
  let edits =
        catMaybes
        (map
         (\opt ->
            (case opt of
               NumEdits x ->
                 case x of
                   Just n -> Just n
                   Nothing -> errorWithoutStackTrace "Bad --num-edits argument"
               _ -> Nothing))
          opts)
  in case (length edits) of
    0 -> 4
    1 -> (head edits)
    _ -> errorWithoutStackTrace "--num-edits may be given zero or one times"
    
-- Take a list of program arguments (as opposed to options); we expect zero (default
-- number of test cases) or one (user specifying the number of test cases explicitly)
parseNumberOfTestCases :: [String] -> Int
parseNumberOfTestCases args =
  case (length args) of
    0 -> 16
    1 -> case (readMaybe (head args) :: Maybe Int) of
           Just n -> n
           Nothing -> errorWithoutStackTrace "Bad argument; try --help"
    _ -> errorWithoutStackTrace "Zero or one arguments"

parseOpts :: [String] -> Options
parseOpts argv
  -- The problem here is that `getOpt` doesn't let us describe our
  -- program arguments in much detail; there's no way, for instance,
  -- to express that `--length` may only be given zero or one times,
  -- or that the program accepts zero or one arguments. Consequently,
  -- we have to handle all that validation here.
 =
  case getOpt Permute options argv of
    (opts, args, []) ->
      if elem Help opts
        then ShowHelp
        else if elem Version opts
               then ShowVersion
               else GenerateTestData
                      (parseStringLength opts, parseNumberOfEdits opts,
                       parseNumberOfTestCases args)
    (_, _, errs) ->
      errorWithoutStackTrace (concat errs ++ usageInfo "Header" options)

main :: IO ()
main = do
  argv <- getArgs
  case (parseOpts argv) of
    ShowHelp -> putStrLn (usageInfo "Header" options)
    ShowVersion ->
      putStrLn ("make-dl-test-data " ++ (showVersion version))
    GenerateTestData (stringLength, numberOfEdits, numberOfTestCases) -> do
      now <- getPOSIXTime
      let g = mkStdGen $ floor $ now
          cases = generateTestData g stringLength numberOfEdits numberOfTestCases
        in mapM_
           (\x -> let (s, t, d) = x
                  in printf "%s\t%s\t%d\n" s t d)
           cases

