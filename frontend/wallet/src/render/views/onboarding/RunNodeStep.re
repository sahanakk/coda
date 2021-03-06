[@bs.scope "window"] [@bs.val] external openExternal: string => unit = "";

module Styles = {
  open Css;
  let loader = style([display(`flex), justifyContent(`center)]);

  let elapsedTime =
    style([
      display(`flex),
      flexDirection(`column),
      alignItems(`center),
      justifyContent(`center),
      width(`rem(12.)),
    ]);
  let downloaderText =
    merge([
      Theme.Text.Header.h3,
      style([color(white), whiteSpace(`nowrap)]),
    ]);
  let smallText =
    merge([
      downloaderText,
      style([
        marginTop(`zero),
        fontSize(`px(13)),
        hover([cursor(`pointer)]),
      ]),
    ]);
};

[@react.component]
let make = (~prevStep, ~createAccount) => {
  <OnboardingTemplate
    heading="Connecting to the Network"
    description={
      <p>
        {React.string(
           "Establishing a connection typically takes between 5-15 minutes.",
         )}
      </p>
    }
    miscLeft=
      <>
        <Spacer height=2.0 />
        <div className=OnboardingTemplate.Styles.buttonRow>
          <Button
            style=Button.HyperlinkBlue2
            label="Go Back"
            onClick={_ => prevStep()}
          />
          <Spacer width=1.5 />
          <Button
            label="Continue"
            style=Button.HyperlinkBlue3
            onClick={_ => createAccount()}
          />
        </div>
      </>
    miscRight={
      <div className=Styles.loader>
        <div className=Styles.elapsedTime>
          <LoaderRing />
          <Spacer height=1.25 />
          <p className=Styles.smallText>
            {React.string("Connecting to the network...")}
          </p>
        </div>
      </div>
    }
  />;
};
