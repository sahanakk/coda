open Tc;

let setTimeout:
  int =>
  ([ | `Canceller(unit => unit)], Task.t('x, [ | `Cancelled | `Finished])) =
  millis => {
    let synchronousCancel = ref(false);
    let cancel: ref(unit => unit) = ref(() => synchronousCancel := true);
    let task =
      Task.uncallbackifyValue(cb =>
        if (synchronousCancel^) {
          cb(`Cancelled);
        } else {
          let token = Js.Global.setTimeout(() => cb(`Finished), millis);
          cancel :=
            (
              () => {
                Js.Global.clearTimeout(token);
                cb(`Cancelled);
              }
            );
        }
      );
    (`Canceller(() => cancel^()), task);
  };

module Window = {
  type t;

  [@bs.val] external current: t = "window";

  [@bs.set] external onClick: (t, unit => unit) => unit = "onclick";
};

module Url = {
  type t;
  [@bs.new] external create: string => t = "URL";

  module SearchParams = {
    type t;
    [@bs.send] external get: (t, string) => string = "";
  };

  [@bs.get] external searchParams: t => SearchParams.t = "";
};

module Navigator = {
  module Clipboard = {
    [@bs.val] [@bs.scope ("navigator", "clipboard")]
    external writeText: string => Js.Promise.t(unit) = "";

    let writeTextTask: string => Task.t('x, unit) =
      str => Task.liftPromise(() => writeText(str));
  };
};

module Stream = {
  module Readable = {
    type t;
    [@bs.send] external on: (t, string, Node.Buffer.t => unit) => unit = "";
  };

  module Writable = {
    type t;
  };
};

module ChildProcess = {
  module Error = {
    [@bs.deriving abstract]
    type t = {
      message: string,
      name: string,
    };
  };

  module Process = {
    [@bs.val] [@bs.module "process"] external env: Js.Dict.t(string) = "";

    [@bs.deriving abstract]
    type t = {
      stdout: Stream.Readable.t,
      stderr: Stream.Readable.t,
    };

    [@bs.send] external kill: (t, string) => unit = "";

    [@bs.send]
    external onError: (t, [@bs.as "error"] _, Error.t => unit) => unit = "on";

    [@bs.send]
    external onExit:
      (
        t,
        [@bs.as "exit"] _,
        (Js.nullable(float), Js.nullable(string)) => unit
      ) =>
      unit =
      "on";

    let onExit = (t, cb) =>
      onExit(t, (f, s) =>
        switch (Js.Nullable.toOption(f), Js.Nullable.toOption(s)) {
        | (None, None) =>
          failwith("Expected one of code,signal to be non-null")
        | (Some(code), None) => cb(`Code(int_of_float(code)))
        | (None, Some(signal)) => cb(`Signal(signal))
        | (Some(_), Some(_)) =>
          failwith("Expected one of code,signal to be null")
        }
      );
    let onExitTask = t => Task.uncallbackifyValue(onExit(t));
  };

  type io;

  /**
    https://nodejs.org/api/child_process.html#child_process_options_stdio
    The stdio parameter to spawn is a 3-element heterogenous list of some string
    keywords and Streams (as well as some other cases not supported here).
    This function lets us safely construct this parameter (with makeIOTriple).
    */
  let makeIO = (v): io =>
    switch (v) {
    | `Pipe => Obj.magic("pipe")
    | `Inherit => Obj.magic("inherit")
    | `Ignore => Obj.magic("ignore")
    | `Stream((s: Stream.Writable.t)) => Obj.magic(s)
    };

  let makeIOTriple = (io1, io2, io3) => (
    makeIO(io1),
    makeIO(io2),
    makeIO(io3),
  );

  let pipe = makeIOTriple(`Pipe, `Pipe, `Pipe);
  let ignore = makeIOTriple(`Ignore, `Ignore, `Ignore);

  [@bs.deriving abstract]
  type spawnOptions = {
    [@bs.optional]
    env: Js.Dict.t(string),
    [@bs.optional]
    shell: string,
    [@bs.optional]
    stdio: (io, io, io),
  };

  [@bs.val] [@bs.module "child_process"]
  external spawn: (string, array(string), spawnOptions) => Process.t =
    "spawn";

  [@bs.val] [@bs.module "child_process"]
  external execSync: (string, array(string)) => unit = "exec";
};

module Fs = {
  [@bs.val] [@bs.module "fs"]
  external readFile:
    (
      string,
      string,
      (Js.Nullable.t(Js.Exn.t), Js.Nullable.t(string)) => unit
    ) =>
    unit =
    "";

  [@bs.val] [@bs.module "fs"]
  external readFileSync: (string, string) => string = "readFileSync";

  [@bs.val] [@bs.module "fs"]
  external openSync: (string, string) => Stream.Writable.t = "";

  [@bs.val] [@bs.module "fs"]
  external writeFile:
    (string, string, string, Js.Nullable.t(Js.Exn.t) => unit) => unit =
    "";

  [@bs.val] [@bs.module "fs"]
  external writeSync: (Stream.Writable.t, string) => unit = "";

  [@bs.val] [@bs.module "fs"]
  external watchFile: (string, unit => unit) => unit = "";
};

module Fetch = {
  [@bs.module] external fetch: ApolloClient.fetch = "node-fetch";
};

module LocalStorage = {
  [@bs.val] [@bs.scope "localStorage"]
  external setItem:
    (
      ~key: [@bs.string] [
              | [@bs.as "network"] `Network
              | [@bs.as "addressbook"] `AddressBook
              | [@bs.as "onboarding"] `Onboarding
            ],
      ~value: string
    ) =>
    unit =
    "";

  [@bs.val] [@bs.scope "localStorage"]
  external getItem:
    (
    [@bs.string]
    [
      | [@bs.as "network"] `Network
      | [@bs.as "addressbook"] `AddressBook
      | [@bs.as "onboarding"] `Onboarding
    ]
    ) =>
    Js.nullable(string) =
    "";
};
