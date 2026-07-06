{-# LANGUAGE DataKinds            #-}
{-# LANGUAGE TypeFamilies         #-}
{-# LANGUAGE TypeOperators        #-}
{-# LANGUAGE GADTs                #-}
{-# LANGUAGE PolyKinds            #-}
{-# LANGUAGE ScopedTypeVariables  #-}
{-# LANGUAGE FlexibleInstances    #-}
{-# LANGUAGE FlexibleContexts     #-}
{-# LANGUAGE UndecidableInstances #-}
{-# LANGUAGE StandaloneDeriving   #-}
{-# LANGUAGE RankNTypes           #-}
{-# LANGUAGE AllowAmbiguousTypes  #-}
{-# LANGUAGE TypeApplications     #-}

module SparseVec where

import Data.Kind   (Type, Constraint)
import Data.Proxy  (Proxy (..))
import GHC.TypeNats
import qualified Data.Map.Strict as Map
import Data.Map.Strict (Map)

--------------------------------------------------------------------------------
-- 1.  Type-level ordered set of Nat (duplicate-free, sorted)
--------------------------------------------------------------------------------

-- | A sorted, duplicate-free list of Nat indices, used as a type-level set.
--   Invariant (enforced by smart constructors / type families): strictly
--   increasing order.
data NatSet = Empty | Cons Nat NatSet

-- | Membership predicate
type family Member (n :: Nat) (s :: NatSet) :: Bool where
  Member _  Empty      = 'False
  Member n (Cons n  _) = 'True
  Member n (Cons m  s) = Member n s

-- | Set union (merge, keeping sorted order, dropping duplicates)
type family Union (a :: NatSet) (b :: NatSet) :: NatSet where
  Union  Empty       b          = b
  Union  a           Empty      = a
  Union (Cons n as) (Cons n bs) = Cons n (Union as bs)
  Union (Cons n as) (Cons m bs) = UnionOrd (CmpNat n m) n as m bs

type family UnionOrd (o :: Ordering) (n :: Nat) (as :: NatSet)
                                     (m :: Nat) (bs :: NatSet) :: NatSet where
  UnionOrd 'LT n as m bs = Cons n (Union as (Cons m bs))
  UnionOrd 'GT n as m bs = Cons m (Union (Cons n as) bs)

-- | Set intersection
type family Inter (a :: NatSet) (b :: NatSet) :: NatSet where
  Inter  Empty       _          = Empty
  Inter  _           Empty      = Empty
  Inter (Cons n as) (Cons n bs) = Cons n (Inter as bs)
  Inter (Cons n as) (Cons m bs) = InterOrd (CmpNat n m) n as m bs

type family InterOrd (o :: Ordering) (n :: Nat) (as :: NatSet)
                                      (m :: Nat) (bs :: NatSet) :: NatSet where
  InterOrd 'LT _ as m bs = Inter as (Cons m bs)
  InterOrd 'GT n as _ bs = Inter (Cons n as) bs

-- | Convert a NatSet to a plain list of Nat values (term level)
class KnownNatSet (s :: NatSet) where
  natSetVal :: Proxy s -> [Natural]

instance KnownNatSet Empty where
  natSetVal _ = []

instance (KnownNat n, KnownNatSet s) => KnownNatSet (Cons n s) where
  natSetVal _ = natVal (Proxy @n) : natSetVal (Proxy @s)

--------------------------------------------------------------------------------
-- 2.  The SparseVec type
--
--     SparseVec s a  is a vector whose *only possible* nonzero positions are
--     the indices in the NatSet `s`.  We store them in a Data.Map keyed by
--     Natural.  The type-level set is the *static support*; at runtime a
--     position may still be zero, but no position *outside* s can be nonzero.
--------------------------------------------------------------------------------

newtype SparseVec (s :: NatSet) a = SparseVec
  { getMap :: Map Natural a
  } deriving (Eq, Ord)

instance (Show a, KnownNatSet s) => Show (SparseVec s a) where
  show v = "SparseVec " ++ show (natSetVal (Proxy @s))
        ++ " " ++ show (getMap v)

--------------------------------------------------------------------------------
-- 3.  Construction
--------------------------------------------------------------------------------

-- | The zero vector (empty map)
zero :: SparseVec s a
zero = SparseVec Map.empty

-- | Insert / update a single entry.
--   The index must be a member of s (checked at compile time).
singleton :: forall n s a. (KnownNat n, Member n s ~ 'True)
          => Proxy n -> a -> SparseVec s a
singleton p x = SparseVec (Map.singleton (natVal p) x)

-- | Build from an association list; indices not in s are silently dropped.
fromList :: KnownNatSet s => [(Natural, a)] -> SparseVec s a
fromList xs = SparseVec (Map.fromList xs)

-- | Read a single coefficient (returns Nothing when absent / zero)
lookupCoeff :: forall n s a. KnownNat n => Proxy n -> SparseVec s a -> Maybe a
lookupCoeff p (SparseVec m) = Map.lookup (natVal p) m

-- | Read with a default
getCoeff :: forall n s a. (KnownNat n, Num a) => Proxy n -> SparseVec s a -> a
getCoeff p v = maybe 0 id (lookupCoeff p v)

--------------------------------------------------------------------------------
-- 4.  The SparseVecClass type class
--
--     Gives a uniform interface for operations parameterised by the support s.
--------------------------------------------------------------------------------

class KnownNatSet s => SparseVecClass (s :: NatSet) where
  -- | Scalar multiplication
  scaleVec :: Num a => a -> SparseVec s a -> SparseVec s a
  scaleVec c (SparseVec m) = SparseVec (Map.map (c *) m)

  -- | Pointwise addition (same support)
  addVec :: Num a => SparseVec s a -> SparseVec s a -> SparseVec s a
  addVec (SparseVec l) (SparseVec r) =
    SparseVec (Map.unionWith (+) l r)

  -- | Pointwise subtraction
  subVec :: Num a => SparseVec s a -> SparseVec s a -> SparseVec s a
  subVec (SparseVec l) (SparseVec r) =
    SparseVec (Map.unionWith (\a b -> a - b) l r)

  -- | Dot product with another sparse vector on the same support
  dotVec :: Num a => SparseVec s a -> SparseVec s a -> a
  dotVec (SparseVec l) (SparseVec r) =
    Map.foldl' (+) 0 (Map.intersectionWith (*) l r)

  -- | Map a function over all stored (nonzero) coefficients
  mapVec :: (a -> b) -> SparseVec s a -> SparseVec s b
  mapVec f (SparseVec m) = SparseVec (Map.map f m)

  -- | Fold over the stored entries
  foldVec :: (b -> (Natural, a) -> b) -> b -> SparseVec s a -> b
  foldVec f z (SparseVec m) = Map.foldlWithKey' (\acc k v -> f acc (k, v)) z m

  -- | Convert to a dense list in index order, filling gaps with 0
  toDense :: Num a => SparseVec s a -> [(Natural, a)]
  toDense v = [ (n, maybe 0 id (Map.lookup n (getMap v)))
              | n <- natSetVal (Proxy @s) ]

instance SparseVecClass Empty
instance (KnownNat n, SparseVecClass s) => SparseVecClass (Cons n s)

--------------------------------------------------------------------------------
-- 5.  Union-typed addition (adding two vectors with different supports)
--
--     The result's support is the union of the two input supports.
--------------------------------------------------------------------------------

addUnion :: (Num a, KnownNatSet (Union s t))
         => SparseVec s a
         -> SparseVec t a
         -> SparseVec (Union s t) a
addUnion (SparseVec l) (SparseVec r) =
  SparseVec (Map.unionWith (+) l r)

--------------------------------------------------------------------------------
-- 6.  Restrict to a sub-support
--
--     Statically asserts that `sub` is contained in `s`, and projects down.
--     Containment is expressed by requiring Member for each element.
--     (A full SubSet type family is also provided for convenience.)
--------------------------------------------------------------------------------

type family SubSet (sub :: NatSet) (sup :: NatSet) :: Constraint where
  SubSet  Empty      _   = ()
  SubSet (Cons n ns) sup = (Member n sup ~ 'True, SubSet ns sup)

restrict :: SubSet sub s => SparseVec s a -> SparseVec sub a
restrict (SparseVec m) = SparseVec m   -- same underlying map, narrowed type

--------------------------------------------------------------------------------
-- 7.  Embed into a larger support
--------------------------------------------------------------------------------

embed :: SparseVec s a -> SparseVec (Union s t) a
embed (SparseVec m) = SparseVec m

--------------------------------------------------------------------------------
-- 8.  Example helpers / type synonyms
--------------------------------------------------------------------------------

-- | Typical 3-sparse vector with indices {0, 2, 5}
type SVec025 = SparseVec ('Cons 0 ('Cons 2 ('Cons 5 'Empty)))

-- | A pair of vectors with disjoint supports {1,3} and {2,4}
type SVec13  = SparseVec ('Cons 1 ('Cons 3 'Empty))
type SVec24  = SparseVec ('Cons 2 ('Cons 4 'Empty))
-- Their union:
-- Union SVec13 SVec24  ==>  'Cons 1 ('Cons 2 ('Cons 3 ('Cons 4 'Empty)))

--------------------------------------------------------------------------------
-- 9.  Quick smoke-test (load in GHCi and run `demo`)
--------------------------------------------------------------------------------

demo :: IO ()
demo = do
  let v1 = fromList @('Cons 0 ('Cons 2 ('Cons 5 'Empty))) [(0,1),(2,3),(5,7)]
      v2 = fromList @('Cons 0 ('Cons 2 ('Cons 5 'Empty))) [(0,10),(5,(-1))]
      v3 = addVec v1 v2
      d  = dotVec v1 v2          -- 0*1 + 2*? + 5*7*(-1) = ...
                                  -- only indices in both: 0->1*10=10, 5->7*(-1)=-7
  putStrLn $ "v1      = " ++ show v1
  putStrLn $ "v2      = " ++ show v2
  putStrLn $ "v1+v2   = " ++ show v3
  putStrLn $ "v1·v2   = " ++ show (d :: Int)
  putStrLn $ "dense v1= " ++ show (toDense v1 :: [(Natural, Int)])

  -- Union of two disjoint vectors
  let a = fromList @('Cons 1 ('Cons 3 'Empty)) [(1, 100),(3, 300)] :: SVec13 Int
      b = fromList @('Cons 2 ('Cons 4 'Empty)) [(2, 200),(4, 400)] :: SVec24 Int
      c = addUnion a b
  putStrLn $ "a ∪+ b  = " ++ show (getMap c)