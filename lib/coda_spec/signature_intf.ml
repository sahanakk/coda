open Core_kernel
open Snark_params

module Private_key = struct
  module type S = sig
    type t
  end
end

module Public_key = struct
  module Base = struct
    module type S = sig
      module Private_key : Private_key.S

      type t [@@deriving sexp]

      val of_private_key_exn : Private_key.t -> t
    end
  end

  module Compressed = struct
    module type S = sig
      type t [@@deriving sexp, bin_io, compare]

      include Comparable.S with type t := t
    end
  end

  module type S = sig
    include Base.S

    module Compressed : Compressed.S

    val compress : t -> Compressed.t
  end
end

module Keypair = struct
  module type S = sig
    module Private_key : Private_key.S
    module Public_key : Public_key.S

    type t = {public_key: Public_key.t; private_key: Private_key.t}
  end
end

module Message = struct
  module type S = sig
    module Snarky_impl : Snarky.Snark_intf
    open Snarky_impl

    type t
    type var

    val hash : t -> nonce:bool list -> Inner_curve.Scalar.t

    val hash_checked :
      var -> nonce:Boolean.var list -> (Inner_curve.Scalar.var, _) Checked.t
  end
end

module type S = sig
  (* TODO: create better snarky interfaces *)
  module Snarky_impl : Snarky.Snark_intf
  open Snarky_impl

  type 'a shifted = (module Inner_curve.Checked.Shifted with type t = 'a)

  module Message : Message.S
    with module Snarky_impl = Snarky_impl

  module Signature : sig
    type t = Inner_curve.Scalar.t * Inner_curve.Scalar.t
    type var = Inner_curve.Scalar.var * Inner_curve.Scalar.var
    val typ : (var, t) Typ.t
    include Sexpable.S with type t := t
  end

  (* TODO: unify keys with primary interfaces *)
  module Private_key : sig
    type t = Inner_curve.Scalar.t
  end

  module Public_key : sig
    type t = Inner_curve.t

    type var = Inner_curve.var
  end

  module Checked : sig
    val compress : Inner_curve.var -> (Boolean.var list, _) Checked.t

    val verification_hash :
         't shifted
      -> Signature.var
      -> Public_key.var
      -> Message.var
      -> (Inner_curve.Scalar.var, _) Checked.t

    val verifies :
         't shifted
      -> Signature.var
      -> Public_key.var
      -> Message.var
      -> (Boolean.var, _) Checked.t

    val assert_verifies :
         't shifted
      -> Signature.var
      -> Public_key.var
      -> Message.var
      -> (unit, _) Checked.t
  end

  val compress : Inner_curve.t -> bool list

  val sign : Private_key.t -> Message.t -> Signature.t

  val shamir_sum : Inner_curve.Scalar.t * Inner_curve.t -> Inner_curve.Scalar.t * Inner_curve.t -> Inner_curve.t

  val verify : Signature.t -> Public_key.t -> Message.t -> bool
end

module Tick = struct
  module type S = S with module Snarky_impl = Tick
end

module Tock = struct
  module type S = S with module Snarky_impl = Tock
end
