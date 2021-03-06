[%%import
"/src/config.mlh"]

open Core_kernel
open Snark_params
open Module_version

module Stable = struct
  module V1 = struct
    module T = struct
      type t = Tock.Proof.t [@@deriving version {asserted}]

      let to_string = Binable.to_string (module Tock_backend.Proof)

      let of_string = Binable.of_string (module Tock_backend.Proof)

      let compare a b = String.compare (to_string a) (to_string b)

      let version_byte = Base58_check.Version_bytes.proof

      let description = "Tock proof"

      (* TODO: Figure out what the right thing to do is for conversion failures *)
      let ( { Bin_prot.Type_class.reader= bin_reader_t
            ; writer= bin_writer_t
            ; shape= bin_shape_t } as bin_t ) =
        Bin_prot.Type_class.cnv Fn.id to_string of_string String.bin_t

      let {Bin_prot.Type_class.read= bin_read_t; vtag_read= __bin_read_t__} =
        bin_reader_t

      let {Bin_prot.Type_class.write= bin_write_t; size= bin_size_t} =
        bin_writer_t
    end

    include T
    include Sexpable.Of_stringable (T)
    module Base58_check = Base58_check.Make (T)

    let to_yojson s = `String (Base58_check.encode (to_string s))

    let of_yojson = function
      | `String s -> (
        match Base58_check.decode s with
        | Ok decoded ->
            Ok (of_string decoded)
        | Error e ->
            Error
              (sprintf "Proof.of_yojson, bad Base58Check: %s"
                 (Error.to_string_hum e)) )
      | _ ->
          Error "Proof.of_yojson expected `String"

    include Registration.Make_latest_version (T)
  end

  module Latest = V1

  module Module_decl = struct
    let name = "coda_base_proof"

    type latest = Latest.t
  end

  module Registrar = Registration.Make (Module_decl)
  module Registered_V1 = Registrar.Register (V1)

  (* see lib/module_version/README-version-asserted.md *)
  module For_tests = struct
    [%%if
    curve_size = 298]

    let%test "proof serialization v1" =
      let proof = Tock_backend.Proof.get_dummy () in
      let known_good_hash =
        "\x22\x36\x54\x9F\x8A\x0A\xBC\x8C\x4E\x90\x22\x91\x30\x20\x4D\x4C\xE1\x11\x01\xD4\xBA\x6B\x38\xEE\x8B\x95\x51\x7E\x6C\x40\xA7\x88"
      in
      Serialization.check_serialization (module V1) proof known_good_hash

    [%%elif
    curve_size = 753]

    let%test "proof serialization v1" =
      let proof = Tock_backend.Proof.get_dummy () in
      let known_good_hash =
        "\xA6\x52\x85\xCE\x46\xC0\xB9\x24\x99\xA2\x3C\x90\x54\xC8\xAE\x7B\x17\x3A\x99\x64\x94\x3A\xE7\x6C\x66\xD9\x48\x5E\x5D\xD4\x83\x3F"
      in
      Serialization.check_serialization (module V1) proof known_good_hash

    [%%else]

    let%test "proof serialization v1" = failwith "No test for this curve size"

    [%%endif]
  end
end

type t = Stable.Latest.t

let dummy = Tock.Proof.dummy

include Sexpable.Of_stringable (Stable.Latest)

[%%define_locally
Stable.Latest.(to_yojson, of_yojson)]
